#include "LwrsFileUnit.hpp"

#include "UNVERSION.hpp"
#include "Encryption/BinaryShuffle.hpp"

#include <stdexcept>

#include "../Const.hpp"
#include "../Core.hpp"
#include "../SecureWipe.hpp"

#include "../../External/utf8proc-2.11.3/utf8proc.h"



#include <algorithm>
#include <cstring>

constexpr size_t SQ_TXT_SIZE = Const::MAX_SEARCH_TEXT;
constexpr size_t HASH_TABLE_SIZE = 256;

namespace {
    uint8_t masterMap[HASH_TABLE_SIZE];
    uint8_t workMap[HASH_TABLE_SIZE];
    size_t g_totalQueryPairs = 0;
    uint32_t g_singleChar = 0; // Character if the length of the query is 1

    inline uint8_t FibonacciBigramHash(uint32_t first, uint32_t second) {
        uint64_t combined = (static_cast<uint64_t>(first) << 32) | second;
        uint64_t hash = combined * 11400714819323198485ULL;
        return static_cast<uint8_t>(hash >> 56);
    }

    inline uint32_t Utf8DecodeAndLower(const char* str, size_t& index) {
        utf8proc_int32_t cp;
        utf8proc_ssize_t bytes_read = utf8proc_iterate(
            reinterpret_cast<const utf8proc_uint8_t*>(str + index),
            -1,
            &cp
        );

        if (bytes_read <= 0) {
            index += 1;
            return 0;
        }

        index += static_cast<size_t>(bytes_read);
        return static_cast<uint32_t>(utf8proc_tolower(cp));
    }
    
    void InitReQ(const char* QQtext) {
        std::memset(masterMap, 0, HASH_TABLE_SIZE);
        g_totalQueryPairs = 0;
        g_singleChar = 0;

        if (!QQtext || QQtext[0] == '\0') return;

        uint32_t qBuf[SQ_TXT_SIZE];
        size_t qLen = 0, qIdx = 0;
        while (QQtext[qIdx] != '\0' && qLen < SQ_TXT_SIZE) {
            uint32_t cp = Utf8DecodeAndLower(QQtext, qIdx);
            if (cp != 0) qBuf[qLen++] = cp;
        }

        if (qLen == 0) return;

        if (qLen == 1) {
            // Request from 1 character
            g_singleChar = qBuf[0];
            return;
        }

        // Query for 2+ characters (bigrams)
        g_totalQueryPairs = qLen - 1;
        for (size_t i = 0; i < g_totalQueryPairs; ++i) {
            uint8_t hashIndex = FibonacciBigramHash(qBuf[i], qBuf[i + 1]);
            if (masterMap[hashIndex] < 255) {
                masterMap[hashIndex]++;
            }
        }
    }

    float QQHashTableSearch256(const char* TextToValid) {
        if (!TextToValid || TextToValid[0] == '\0') return 0.0f;

        // =========================================================
        // BRANCH 1: Searching for a single character with decreasing coefficient
        // =========================================================
        if (g_totalQueryPairs == 0) {
            if (g_singleChar == 0) return 0.0f; // Empty query

            size_t tIdx = 0;
            size_t charPosition = 0; // Order number of the character in the text

            while (TextToValid[tIdx] != '\0' && charPosition < Const::PU_DESC_LEN) {
                uint32_t cp = Utf8DecodeAndLower(TextToValid, tIdx);
                if (cp == 0) continue;

                if (cp == g_singleChar) {
                    // Character found at position `charPosition` (0, 1, 2...)
                    // Formula: 1.0 / (1 + k * pos)
                    // k = 0.1f controls the falling speed of the graph:
                    // Pos  0 -> 1.000 (at the very start)
                    // Pos  1 -> 0.909
                    // Pos  5 -> 0.666
                    // Pos 50 -> 0.166
                    // Pos 99 -> 0.091 (approaches 0, but never reaches 0)
                    constexpr float k = 0.1f;
                    return 1.0f / (1.0f + k * static_cast<float>(charPosition));
                }

                charPosition++;
            }

            return 0.0f; // Symbol is not found in the text
        }

        // =========================================================
        // CASE 2: Standard search of bigrams (for queries of 2 characters)
        // =========================================================
        std::memcpy(workMap, masterMap, HASH_TABLE_SIZE);

        size_t tIdx = 0;
        uint32_t prevCp = Utf8DecodeAndLower(TextToValid, tIdx);
        if (prevCp == 0) return 0.0f;

        size_t matches = 0;
        size_t totalTextChars = 1;

        while (TextToValid[tIdx] != '\0' && totalTextChars < Const::PU_DESC_LEN) {
            uint32_t currCp = Utf8DecodeAndLower(TextToValid, tIdx);
            if (currCp == 0) continue;

            totalTextChars++;
            uint8_t hashIndex = FibonacciBigramHash(prevCp, currCp);

            if (workMap[hashIndex] > 0) {
                matches++;
                workMap[hashIndex]--;
            }

            prevCp = currCp;
        }

        return static_cast<float>(matches) / static_cast<float>(g_totalQueryPairs);
    }

}

// ----------------------------------------------------------------------------------

void PU::CopyParam(const char* NEW_name, const char* NEW_pass, const char* NEW_desc) {
     if (NEW_name) {
         SecureWipe(name, Const::PU_NAME_LEN + 1);
         std::strncpy(name, NEW_name, Const::PU_NAME_LEN);
         name[Const::PU_NAME_LEN] = '\0';
     }
     if (NEW_pass) {
         SecureWipe(pass, Const::PU_PASS_LEN + 1);
         std::strncpy(pass, NEW_pass, Const::PU_PASS_LEN);
         pass[Const::PU_PASS_LEN] = '\0';
     }
     if (NEW_desc) {
         SecureWipe(desc, Const::PU_DESC_LEN + 1);
         std::strncpy(desc, NEW_desc, Const::PU_DESC_LEN);
         desc[Const::PU_DESC_LEN] = '\0';
     }
}

void PU::Clear() {
	SecureWipe(name, Const::PU_NAME_LEN + 1);
	SecureWipe(pass, Const::PU_PASS_LEN + 1);
	SecureWipe(desc, Const::PU_DESC_LEN + 1);
}

// ----------------------------------------------------------------------------------

PU*			LwrsFileUnit::PU_arr		= nullptr;
size_t		LwrsFileUnit::Size_PU_arr	= 0;

uint8_t*    LwrsFileUnit::PINSALT       = nullptr;
size_t      LwrsFileUnit::PINSALT_size  = 0;
uint8_t*    LwrsFileUnit::PINHASH       = nullptr;
size_t      LwrsFileUnit::PINHASH_size  = 0;

char		LwrsFileUnit::version[18]			= { "\0" };
size_t		LwrsFileUnit::filedata_BYTE         = 0;
timer		LwrsFileUnit::t_from_start          = timer{};
float       LwrsFileUnit::ShannonEntropy08      = 0.0f;
int64_t		LwrsFileUnit::ns_agron2				= 0;
int64_t		LwrsFileUnit::ns_XOR_MASK_Suffle			= 0;
int64_t		LwrsFileUnit::ns_InsidePU_XOR_MASK_Suffle	= 0;

char* LwrsFileUnit::GetVersionTEXT() {
	return version;
}
size_t	LwrsFileUnit::GetFileSizeBYTE() {
	return filedata_BYTE;
}
int64_t	LwrsFileUnit::GetNs_FromOpen() {
	return t_from_start.GetTime_ns();
}
float	LwrsFileUnit::Get_Entropy08() {
    return ShannonEntropy08;
}
int64_t	LwrsFileUnit::GetNs_Argon2() {
	return ns_agron2;
}
int64_t	LwrsFileUnit::GetNs_XOR_MASK_Suffle() {
	return ns_XOR_MASK_Suffle;
}
int64_t	LwrsFileUnit::GetNs_InsidePU_XOR_MASK_Suffle() {
	return ns_InsidePU_XOR_MASK_Suffle;
}





size_t LwrsFileUnit::GetPUCount() {
	return Size_PU_arr;
}

const PU& LwrsFileUnit::GetPUByfloatID(int id) {
	if (id < 0 || id >= Size_PU_arr) {
        throw std::out_of_range("Invalid index in GetPUByfloatID");
	}
	return PU_arr[id];
}
void LwrsFileUnit::AddPU(const char* NEW_name, const char* NEW_pass, const char* NEW_desc) {
    size_t newSize = Size_PU_arr + 1;
    PU* newArray = new PU[newSize];

    // ADD NEW PU IN 0 ID
    newArray[0].CopyParam(NEW_name, NEW_pass, NEW_desc);
    newArray[0].ConstID = 0;

    if (PU_arr) {
        for (size_t i = 1; i < newSize; ++i) {
            newArray[i] = PU_arr[i-1];
            newArray[i].ConstID++;
        }

        for (size_t i = 0; i < Size_PU_arr; ++i) {
            PU_arr[i].Clear();
        }
        delete[] PU_arr;
    }

    PU_arr = newArray;
    Size_PU_arr = newSize;
}
void LwrsFileUnit::AddPU_Back(const char* NEW_name, const char* NEW_pass, const char* NEW_desc) {
    size_t newSize = Size_PU_arr + 1;
    PU* newArray = new PU[newSize];

    if (PU_arr) {
        for (size_t i = 0; i < Size_PU_arr; ++i) {
            newArray[i] = PU_arr[i];
        }

        for (size_t i = 0; i < Size_PU_arr; ++i) {
            PU_arr[i].Clear();
        }
        delete[] PU_arr;
    }

    newArray[Size_PU_arr].CopyParam(NEW_name, NEW_pass, NEW_desc);
    newArray[Size_PU_arr].ConstID = static_cast<int>(Size_PU_arr);

    PU_arr = newArray;
    Size_PU_arr = newSize;
}
void LwrsFileUnit::ChangePU(int const_id, const char* NEW_name, const char* NEW_pass, const char* NEW_desc) {
    if (const_id < 0 || const_id >= Size_PU_arr) {
        return;
    }
    for (int i = 0; i < Size_PU_arr; ++i) {
        if (PU_arr[i].ConstID == const_id) {
            PU_arr[i].CopyParam(NEW_name, NEW_pass, NEW_desc);
            return;
        }
    }
}
void LwrsFileUnit::DeletePU(int const_id) {
    int rID = -1;
    for (size_t i = 0; i < Size_PU_arr; ++i) {
        if (PU_arr[i].ConstID == const_id) {
            rID = static_cast<int>(i);
            break;
        }
    }

    if (rID == -1) return;

    PU_arr[rID].Clear();

    size_t newSize = Size_PU_arr - 1;

    if (newSize == 0) {
        delete[] PU_arr;
        PU_arr = nullptr;
        Size_PU_arr = 0;
        return;
    }

    PU* newArray = new PU[newSize];

    size_t writeIndex = 0;
    for (size_t i = 0; i < Size_PU_arr; ++i) {
        if (static_cast<int>(i) == rID) continue;

        newArray[writeIndex] = PU_arr[i];

        if (newArray[writeIndex].ConstID > const_id) {
            newArray[writeIndex].ConstID--;
        }

        writeIndex++;
    }

    for (size_t i = 0; i < Size_PU_arr; ++i) {
        PU_arr[i].Clear();
    }
    delete[] PU_arr;

    PU_arr = newArray;
    Size_PU_arr = newSize;
}


bool LwrsFileUnit::MovePU(int const_id, bool isUPmove) {
    if (const_id < 0 || const_id >= Size_PU_arr) {
        return false;
    }
    if (isUPmove) {
        if (const_id == 0) return false;
        int target_id = const_id - 1;
        return MovePUtoID(const_id, target_id);
    }
    else {
        if (const_id == Size_PU_arr - 1) return false;
        int target_id = const_id + 1;
        return MovePUtoID(const_id, target_id);
    }
    return true;
}
bool LwrsFileUnit::MovePUtoID(int const_id, int const_id_target) {
    if (const_id < 0 || const_id >= Size_PU_arr) return false;
    if (const_id_target < 0 || const_id_target >= Size_PU_arr) return false;
    if (const_id == const_id_target) return false;

    for (size_t i = 0; i < Size_PU_arr; ++i) {
        int old_id = PU_arr[i].ConstID;

        if (old_id == const_id) {
            PU_arr[i].ConstID = const_id_target;
        }
        else if (const_id > const_id_target) {
            if (old_id >= const_id_target && old_id < const_id) {
                PU_arr[i].ConstID++;
            }
        }
        else if (const_id < const_id_target) {
            if (old_id > const_id && old_id <= const_id_target) {
                PU_arr[i].ConstID--;
            }
        }
    }
    return true;
}


void LwrsFileUnit::NormalizePUarr() {
    if (!PU_arr || Size_PU_arr <= 1) return;

    // (Selection Sort)
    for (size_t i = 0; i < Size_PU_arr - 1; ++i) {
        size_t minIdx = i;
        for (size_t j = i + 1; j < Size_PU_arr; ++j) {
            if (PU_arr[j].ConstID < PU_arr[minIdx].ConstID) {
                minIdx = j;
            }
        }

        if (minIdx != i) {
            PU temp = PU_arr[i];
            PU_arr[i] = PU_arr[minIdx];
            PU_arr[minIdx] = temp;

            temp.Clear();
        }
    }
}
void LwrsFileUnit::QQ_Search(const char* QQ_txt, bool is_nameQ, bool is_descQ) {
    if ((!is_nameQ && !is_descQ) || Size_PU_arr == 0) return;
    if (QQ_txt == nullptr || QQ_txt[0] == '\0') {
        NormalizePUarr();
        return;
    }

    InitReQ(QQ_txt);

    for (int i = 0; i < Size_PU_arr; ++i) {
        PU_arr[i].sortVal = 0.0f;

        if (is_nameQ) {
            PU_arr[i].sortVal += QQHashTableSearch256(PU_arr[i].name);
        }
        if (is_descQ) {
            PU_arr[i].sortVal += QQHashTableSearch256(PU_arr[i].desc);
        }
    }

    // sort
    for (size_t i = 0; i < Size_PU_arr - 1; ++i) {
        size_t maxIdx = i;
        for (size_t j = i + 1; j < Size_PU_arr; ++j) {
            if (PU_arr[j].sortVal > PU_arr[maxIdx].sortVal) {
                maxIdx = j;
            }
        }

        if (maxIdx != i) {
            PU temp = PU_arr[i];
            PU_arr[i] = PU_arr[maxIdx];
            PU_arr[maxIdx] = temp;

            temp.Clear();
        }
    }
}


bool LwrsFileUnit::isPINreal(const uint8_t(&PIN)[4]) {
    return UNVERSION::CheckPIN(PIN, PINSALT, PINSALT_size, PINHASH, PINHASH_size);
}

LwrsStatusOperation LwrsFileUnit::FillSelf(
    const uint8_t* data, size_t data_size,
    const uint8_t* MPASS, size_t MPASS_SIZE
) {

    uint16_t Vmain = 0; uint16_t Vfile = 0; uint16_t Vadd = 0;

    LwrsStatusOperation ret = UNVERSION::UNV_ENCODE_TO_PU(
        data, data_size, MPASS, MPASS_SIZE,
        Vmain, Vfile, Vadd,
        ShannonEntropy08,
        ns_agron2,
        ns_XOR_MASK_Suffle,
        ns_InsidePU_XOR_MASK_Suffle,
        PINSALT, PINSALT_size,
        PINHASH, PINHASH_size
    );

    snprintf(version, sizeof(version), "%u.%u.%u", Vmain, Vfile, Vadd);
    filedata_BYTE = data_size;

    if (ret == LwrsStatusOperation::OK || ret == LwrsStatusOperation::DownVersion) {
        t_from_start.Start();
    }
    return ret;
}
LwrsStatusOperation LwrsFileUnit::FillSelf_DOUBLE(
    const uint8_t* data0, size_t data0_size,
    const uint8_t* data1, size_t data1_size,
    const uint8_t* MPASS, size_t MPASS_SIZE
) {
    if (data0_size != data1_size) return LwrsStatusOperation::ErrorSize;

    Core::DROPFRAME_ENCODE(DecodeStatus::BinShuffle); // draw frame wait 1/60s
    size_t TEMPRawDataSize = 0;
    uint8_t* TEMPRawData = BinaryShuffle::UN_Shuffle(data0, data1, data0_size, TEMPRawDataSize);


    uint16_t Vmain = 0; uint16_t Vfile = 0; uint16_t Vadd = 0;

    LwrsStatusOperation ret = UNVERSION::UNV_ENCODE_TO_PU(
        TEMPRawData, TEMPRawDataSize, MPASS, MPASS_SIZE,
        Vmain, Vfile, Vadd,
        ShannonEntropy08,
        ns_agron2,
        ns_XOR_MASK_Suffle,
        ns_InsidePU_XOR_MASK_Suffle,
        PINSALT, PINSALT_size,
        PINHASH, PINHASH_size
    );
    SecureWipe(TEMPRawData, TEMPRawDataSize);
    delete[] TEMPRawData; TEMPRawData = nullptr;

    snprintf(version, sizeof(version), "%u.%u.%u", Vmain, Vfile, Vadd);
    filedata_BYTE = TEMPRawDataSize;

    if (ret == LwrsStatusOperation::OK || ret == LwrsStatusOperation::DownVersion) {
        t_from_start.Start();
    }
    return ret;
}

uint8_t* LwrsFileUnit::GetDataForSave(
    size_t& new_data_size, const uint8_t(&PIN)[4],
    const uint8_t* MPASS, size_t MPASS_SIZE
) {
    return UNVERSION::UNV_GET_DATA_CAST(new_data_size, PIN, MPASS, MPASS_SIZE);
}

void LwrsFileUnit::DATA_DOUBLE(
    const uint8_t* DATA, size_t DATAsize,
    uint8_t*& data0, size_t& new_d0_size,
    uint8_t*& data1, size_t& new_d1_size
) {
    Core::DROPFRAME_ENCODE(DecodeStatus::BinShuffle); // draw frame wait 1/60s
    size_t new_size = BinaryShuffle::Shuffle(DATA, DATAsize, data0, data1);
    new_d0_size = new_size;
    new_d1_size = new_size;
}


void LwrsFileUnit::ClearSelf() {
    for (size_t i = 0; i < Size_PU_arr; ++i) {
        PU_arr[i].Clear();
    }
    delete[] PU_arr;    PU_arr = nullptr;
    Size_PU_arr = 0;

    SecureWipe(PINSALT, PINSALT_size);
    SecureWipe(PINHASH, PINHASH_size);
    delete[] PINSALT;   PINSALT = nullptr;
    delete[] PINHASH;   PINHASH = nullptr;
    PINSALT_size = 0;   PINHASH_size = 0;

    // delete addit var
    version[0] = '\0';
    filedata_BYTE = 0;
    ShannonEntropy08 = 0.0f;
    ns_agron2 = 0;
    ns_XOR_MASK_Suffle = 0;
    ns_InsidePU_XOR_MASK_Suffle = 0;

}