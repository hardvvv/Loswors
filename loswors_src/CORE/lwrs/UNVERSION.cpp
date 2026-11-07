/*
FILE:

	version
	! WARNING !
	PIN salt
	PIN hash
	master salt
	...
	[ data ]
	...
	File hash check

// ---------------------------------

	where [data] = array of PU 

	where PU = :
		NAME salt
		NAME
		PASS salt
		PASS
		DESC salt
		DESC

*/

#include "UNVERSION.hpp"

#include <string>
#include <vector>

#include <cstdint>
#include <cstddef>
#include <cmath>



#include "../Core.hpp"

#include "../Encode/UTF8_to_NFC.hpp"

#include "../SecureWipe.hpp"
#include "Encryption/FileHash.hpp"
#include "Encryption/KDF.hpp"
#include "Encryption/XOR_Shuffle.hpp"

constexpr double LOG2_E = 1.44269504088896340736;

// (sizeof(uint16_t) * 3) -> version

constexpr size_t
	MIN_FILE_SIZE =	(sizeof(uint16_t) * 3) +
					Const::FILE_WARNING_SIZE +
					Const::PIN_SALT_SIZE +
					Const::PIN_HASH_SIZE +
					Const::MASTER_PASS_SALT_SIZE + 
					Const::FILE_CHECK_HASH_SIZE,

	OF_TO_DATA = MIN_FILE_SIZE - Const::FILE_CHECK_HASH_SIZE,

	OF_TO_PINSALT = (sizeof(uint16_t) * 3) + Const::FILE_WARNING_SIZE,
	OF_TO_PINHASH = OF_TO_PINSALT + Const::PIN_SALT_SIZE,

	OF_TO_MASTER_SALT =	(sizeof(uint16_t) * 3) +
						Const::FILE_WARNING_SIZE +
						Const::PIN_SALT_SIZE +
						Const::PIN_HASH_SIZE,

	SIZE_ONE_PU =	Const::PU_NAME_SALT_SIZE +
					Const::PU_NAME_LEN +
					Const::PU_PASS_SALT_SIZE +
					Const::PU_PASS_LEN +
					Const::PU_DESC_SALT_SIZE +
					Const::PU_DESC_LEN;
					

namespace {

	float GetShannonEntropy08(const uint8_t* data, size_t size) {
		if (!data || size == 0) {
			return 0.0f;
		}

		size_t frequencies[256] = { 0 };
		for (size_t i = 0; i < size; ++i) {
			frequencies[data[i]]++;
		}

		double entropy = 0.0;
		const double invSize = 1.0 / static_cast<double>(size);

		for (int i = 0; i < 256; ++i) {
			if (frequencies[i] > 0) {
				double p = static_cast<double>(frequencies[i]) * invSize;

				entropy -= p * (std::log(p) * LOG2_E); // LOG2_E = (1 / ln(2))
			}
		}

		return static_cast<float>(entropy);
	}

};

namespace UNVERSION {

	bool CheckPIN(
		const uint8_t(&PIN)[4],
		const uint8_t* PINsalt, size_t PINsalt_size,
		const uint8_t* PINhash, size_t PINhash_size
	) {
		if (!PINsalt || !PINhash || PINsalt_size == 0 || PINhash_size == 0) return false;

		uint8_t* T_HASH = new uint8_t[PINhash_size];
		SecureWipe(T_HASH, PINhash_size);

		KDF::Sha256_PIN_Interface(PIN, 4, PINsalt, PINsalt_size, T_HASH, PINhash_size);

		bool ret = true;
		for (size_t i = 0; i < PINhash_size; ++i) {
			if (T_HASH[i] != PINhash[i]) {
				ret = false;
				break;
			}
		}
		SecureWipe(T_HASH, PINhash_size);
		delete[] T_HASH; T_HASH = nullptr;
		return ret;
	}

	LwrsStatusOperation UNV_ENCODE_TO_PU(
		const uint8_t* data, size_t data_size, 
		const uint8_t* MPASS, size_t MPASS_SIZE,
		uint16_t& Vmain, uint16_t& Vfile, uint16_t& Vadd,
		float& Entropy08,
		int64_t& Ns_Argon2,
		int64_t& Ns_XOR_MASK_Suffle,
		int64_t& Ns_InsidePU_XOR_MASK_Suffle,
		uint8_t*& PINsalt, size_t& PINsalt_size,
		uint8_t*& PINhash, size_t& PINhash_size
	) {

		if (!data || data_size < MIN_FILE_SIZE) {
			return LwrsStatusOperation::ErrorSize;
		}
		// FIRST Check vercion, AFTER check file hash (Different versions may have different hash sizes)

		// ----------
		// Read Version
		// -----------------------------------------------------------------
		size_t OF = 0; // offset for read
		memcpy(&Vmain, data + OF, sizeof(uint16_t));	OF += sizeof(uint16_t);
		memcpy(&Vfile, data + OF, sizeof(uint16_t));	OF += sizeof(uint16_t);
		memcpy(&Vadd, data + OF, sizeof(uint16_t));		OF += sizeof(uint16_t);
		
		if (Vfile > Const::LOSWORS_VERSION_INT[1]) {
			return LwrsStatusOperation::ErrorUpVersion;
		}

		// ----------
		// Hash Check
		// -----------------------------------------------------------------
		Core::DROPFRAME_ENCODE(DecodeStatus::FileHash);
		if (!FileHash::IS_FILE_HASH_OK(data, data_size, Const::FILE_CHECK_HASH_SIZE)) {
			return LwrsStatusOperation::ErrorFormat;
		}

		// ----------
		// Size Check
		// -----------------------------------------------------------------
		size_t CountPU = 0;
		if ((data_size - MIN_FILE_SIZE) % SIZE_ONE_PU != 0) {
			return LwrsStatusOperation::ErrorSize;
		}
		else {
			CountPU = (data_size - MIN_FILE_SIZE) / SIZE_ONE_PU;
		}

		// ----------
		// Decode
		// -----------------------------------------------------------------

		// ENTROPY COUNT
		size_t IdataSize = CountPU * SIZE_ONE_PU;
		Entropy08 = GetShannonEntropy08(data + OF_TO_DATA, IdataSize);

		// ARGON2
		Core::DROPFRAME_ENCODE(DecodeStatus::Argon2);
		uint8_t MASTER_HASH[Const::MASTER_HASH_SIZE] = {};
		SecureWipe(MASTER_HASH, Const::MASTER_HASH_SIZE);
		timer tt = timer();

		if (!KDF::Argon2Interface(
			MPASS, MPASS_SIZE,
			data + OF_TO_MASTER_SALT, Const::MASTER_PASS_SALT_SIZE,
			MASTER_HASH, Const::MASTER_HASH_SIZE
		)) {
			SecureWipe(MASTER_HASH, Const::MASTER_HASH_SIZE);
			return LwrsStatusOperation::ErrorARGON2;
		}
		Ns_Argon2 = tt.GetTime_ns_Reset();


		auto PINsave = [&]() {
			PINsalt_size = Const::PIN_SALT_SIZE;
			PINhash_size = Const::PIN_HASH_SIZE;

			// pin hash from file
			delete[] PINhash; PINhash = new uint8_t[PINhash_size];
			memcpy(PINhash, data + OF_TO_PINHASH, PINhash_size);

			// pin salt first iteration
			delete[] PINsalt; PINsalt = new uint8_t[PINsalt_size]; SecureWipe(PINsalt, PINsalt_size);
			KDF::Sha256_PIN_Interface(
				MASTER_HASH, Const::MASTER_HASH_SIZE,
				data + OF_TO_PINSALT, PINsalt_size,
				PINsalt, PINsalt_size
			);
		};

		// PIN SAVE
		if (CountPU == 0) {
			Ns_XOR_MASK_Suffle = 0;
			Ns_InsidePU_XOR_MASK_Suffle = 0;
			PINsave();
			SecureWipe(MASTER_HASH, Const::MASTER_HASH_SIZE);
			return LwrsStatusOperation::OK;
		}
		
		// XOR SHUFFLE
		Core::DROPFRAME_ENCODE(DecodeStatus::G_XOR_Shuffle);
		tt.Start();
		uint8_t* Idata = new uint8_t[IdataSize]; // IdataSize > 0 ! after PIN SAVE
		memcpy(Idata, data + OF_TO_DATA, IdataSize);

		XOR_Shuffle::for_GDATA_OUT(Idata, IdataSize, MASTER_HASH, Const::MASTER_HASH_SIZE);
		Ns_XOR_MASK_Suffle = tt.GetTime_ns_Reset();

		// INSIDE PU XOR SHUFFLE
		Core::DROPFRAME_ENCODE(DecodeStatus::I_XOR_Shuffle);
		tt.Start();
		/*
			uint8_t name_salt[]
			uint8_t name[]

			uint8_t pass_salt[]
			uint8_t pass[]

			uint8_t desc_salt[]
			uint8_t desc[]
		*/
		uint8_t INSIDE_HASH[Const::PU_HASH_SIZE] = {};
		for (size_t i = 0; i < CountPU; ++i) {
			size_t OF_CUR = i * SIZE_ONE_PU;
			
			// NAME ----------------------
			SecureWipe(		INSIDE_HASH,		Const::PU_HASH_SIZE);
			KDF::Harpid(	MASTER_HASH,		Const::MASTER_HASH_SIZE,
							Idata + OF_CUR,		Const::PU_NAME_SALT_SIZE,
							INSIDE_HASH,		Const::PU_HASH_SIZE);

			OF_CUR += Const::PU_NAME_SALT_SIZE;
			XOR_Shuffle::for_INSIDE_PU_OUT(Idata + OF_CUR, Const::PU_NAME_LEN, INSIDE_HASH, Const::PU_HASH_SIZE);
			OF_CUR += Const::PU_NAME_LEN;

			// PASS ----------------------
			SecureWipe(		INSIDE_HASH,		Const::PU_HASH_SIZE);
			KDF::Harpid(	MASTER_HASH,		Const::MASTER_HASH_SIZE,
							Idata + OF_CUR,		Const::PU_PASS_SALT_SIZE,
							INSIDE_HASH,		Const::PU_HASH_SIZE);

			OF_CUR += Const::PU_PASS_SALT_SIZE;
			XOR_Shuffle::for_INSIDE_PU_OUT(Idata + OF_CUR, Const::PU_PASS_LEN, INSIDE_HASH, Const::PU_HASH_SIZE);
			OF_CUR += Const::PU_PASS_LEN;

			// DESC ----------------------
			SecureWipe(		INSIDE_HASH,		Const::PU_HASH_SIZE);
			KDF::Harpid(	MASTER_HASH,		Const::MASTER_HASH_SIZE,
							Idata + OF_CUR,		Const::PU_DESC_SALT_SIZE,
							INSIDE_HASH,		Const::PU_HASH_SIZE);

			OF_CUR += Const::PU_DESC_SALT_SIZE;
			XOR_Shuffle::for_INSIDE_PU_OUT(Idata + OF_CUR, Const::PU_DESC_LEN, INSIDE_HASH, Const::PU_HASH_SIZE);
			OF_CUR += Const::PU_DESC_LEN;
		}
		SecureWipe(INSIDE_HASH, Const::PU_HASH_SIZE);
		Ns_InsidePU_XOR_MASK_Suffle = tt.GetTime_ns_Reset();

		// CALL ADD
		char NAME[Const::PU_NAME_LEN + 1] = {};	SecureWipe(NAME, Const::PU_NAME_LEN + 1);
		char PASS[Const::PU_PASS_LEN + 1] = {};	SecureWipe(PASS, Const::PU_PASS_LEN + 1);
		char DESC[Const::PU_DESC_LEN + 1] = {};	SecureWipe(DESC, Const::PU_DESC_LEN + 1);

		auto ToSingleLine = [](char* text, size_t size) {
			if (!text) return;
			for (size_t i = 0; i < size; ++i) {
				if (text[i] == '\n' || text[i] == '\r') {
					text[i] = ' ';
				}
			}
		};

		for (size_t i = 0; i < CountPU; ++i) {
			size_t OF_CUR = i * SIZE_ONE_PU;

			// NAME ----------------------
			OF_CUR += Const::PU_NAME_SALT_SIZE;
			UTF8_to_NFC::to_clean_string(Idata + OF_CUR, Const::PU_NAME_LEN, NAME, Const::PU_NAME_LEN + 1);
			ToSingleLine(NAME, Const::PU_NAME_LEN + 1);
			OF_CUR += Const::PU_NAME_LEN;

			// PASS ----------------------
			OF_CUR += Const::PU_PASS_SALT_SIZE;
			UTF8_to_NFC::to_clean_string(Idata + OF_CUR, Const::PU_PASS_LEN, PASS, Const::PU_PASS_LEN + 1);
			ToSingleLine(PASS, Const::PU_PASS_LEN + 1);
			OF_CUR += Const::PU_PASS_LEN;

			// DESC ----------------------
			OF_CUR += Const::PU_DESC_SALT_SIZE;
			UTF8_to_NFC::to_clean_string(Idata + OF_CUR, Const::PU_DESC_LEN, DESC, Const::PU_DESC_LEN + 1);
			OF_CUR += Const::PU_DESC_LEN;


			LwrsFileUnit::AddPU_Back(NAME, PASS, DESC);
			SecureWipe(NAME, Const::PU_NAME_LEN + 1);
			SecureWipe(PASS, Const::PU_PASS_LEN + 1);
			SecureWipe(DESC, Const::PU_DESC_LEN + 1);
		}

		SecureWipe(Idata, IdataSize);
		delete[] Idata; Idata = nullptr;

		// ----------
		// Fill pin data
		// -----------------------------------------------------------------
		PINsave();
		SecureWipe(MASTER_HASH, Const::MASTER_HASH_SIZE);
		return LwrsStatusOperation::OK;
	}

	uint8_t* UNV_GET_DATA_CAST(
		size_t& new_data_size, const uint8_t(&PIN)[4],
		const uint8_t* MPASS, size_t MPASS_SIZE
	) {
		size_t PUcount = LwrsFileUnit::GetPUCount();
		size_t DATAsz = MIN_FILE_SIZE + (PUcount * SIZE_ONE_PU);

		uint8_t* DATA = new uint8_t[DATAsz]; SecureWipe(DATA, DATAsz);
		size_t OF = 0; // offset

		// VERSION
		// --------------------------------------------------------------------
		memcpy(DATA + OF, Const::LOSWORS_VERSION_INT, sizeof(uint16_t) * 3);
		OF += sizeof(uint16_t) * 3;

		// WARNING
		// --------------------------------------------------------------------
		memcpy(DATA + OF, Const::FILE_WARNING_BYTES, Const::FILE_WARNING_SIZE);
		OF += Const::FILE_WARNING_SIZE;

		size_t OffsetToPIN_SALTHASH = OF;
		OF += Const::PIN_SALT_SIZE;
		OF += Const::PIN_HASH_SIZE;
		
		// MASTER SALT
		// --------------------------------------------------------------------
		uint8_t* MASTER_SALT = KDF::RandomSalt(Const::MASTER_PASS_SALT_SIZE);
		memcpy(DATA + OF, MASTER_SALT, Const::MASTER_PASS_SALT_SIZE);
		OF += Const::MASTER_PASS_SALT_SIZE;

		// ARGON2 ---
		Core::DROPFRAME_ENCODE(DecodeStatus::Argon2);
		uint8_t MASTER_HASH[Const::MASTER_HASH_SIZE] = {}; SecureWipe(MASTER_HASH, Const::MASTER_HASH_SIZE);
		if (!KDF::Argon2Interface(MPASS, MPASS_SIZE, MASTER_SALT, Const::MASTER_PASS_SALT_SIZE, MASTER_HASH, Const::MASTER_HASH_SIZE)) {

			SecureWipe(MASTER_HASH, Const::MASTER_HASH_SIZE);
			SecureWipe(MASTER_SALT, Const::MASTER_PASS_SALT_SIZE);	delete[] MASTER_SALT; MASTER_SALT = nullptr;
			SecureWipe(DATA, DATAsz);								delete[] DATA; DATA = nullptr;
			new_data_size = 0;
			return nullptr;
		}


		// PIN SAVE
		// --------------------------------------------------------------------
		// sha256(sha256(PIN_SALT, MASTER_HASH), PIN) where sha256(salt, pass)
		uint8_t* PINsalt = KDF::RandomSalt(Const::PIN_SALT_SIZE);
		memcpy(DATA + OffsetToPIN_SALTHASH, PINsalt, Const::PIN_SALT_SIZE);
		OffsetToPIN_SALTHASH += Const::PIN_SALT_SIZE;

		uint8_t* tmp_PINsalt = new uint8_t[Const::PIN_SALT_SIZE]; SecureWipe(tmp_PINsalt, Const::PIN_SALT_SIZE);

		KDF::Sha256_PIN_Interface(
			MASTER_HASH, Const::MASTER_HASH_SIZE,
			PINsalt, Const::PIN_SALT_SIZE,
			tmp_PINsalt, Const::PIN_SALT_SIZE
		);
		KDF::Sha256_PIN_Interface(
			PIN, 4,
			tmp_PINsalt, Const::PIN_SALT_SIZE,
			DATA + OffsetToPIN_SALTHASH, Const::PIN_HASH_SIZE
		);
		
		SecureWipe(PINsalt, Const::PIN_SALT_SIZE); delete[] PINsalt; PINsalt = nullptr;
		SecureWipe(tmp_PINsalt, Const::PIN_SALT_SIZE); delete[] tmp_PINsalt; tmp_PINsalt = nullptr;


		// WRITE PU
		// --------------------------------------------------------------------
		auto ENDxxFHASH = [&](size_t OFSET) {
			Core::DROPFRAME_ENCODE(DecodeStatus::FileHash);

			SecureWipe(MASTER_SALT, Const::MASTER_PASS_SALT_SIZE); delete[] MASTER_SALT; MASTER_SALT = nullptr;

			uint8_t* F_hash = FileHash::MAKE_FILE_HASH(DATA, OFSET, Const::FILE_CHECK_HASH_SIZE);
			memcpy(DATA + OFSET, F_hash, Const::FILE_CHECK_HASH_SIZE);
			OFSET += Const::FILE_CHECK_HASH_SIZE;

			SecureWipe(F_hash, Const::FILE_CHECK_HASH_SIZE); delete[] F_hash; F_hash = nullptr;

			if (OFSET != DATAsz) {
				SecureWipe(DATA, DATAsz);
				delete[] DATA; DATA = nullptr;
				new_data_size = 0;
				return;
			}
			new_data_size = DATAsz;
		};

		if (PUcount == 0) {
			ENDxxFHASH(OF);
			return DATA;
		}

		// if PU exist
		std::vector<int> links(PUcount);
		for (size_t i = 0; i < PUcount; ++i) {
			size_t cid = LwrsFileUnit::GetPUByfloatID(i).ConstID;
			if (cid < PUcount) links[cid] = i;
		}
		uint8_t INSIDE_HASH[Const::PU_HASH_SIZE] = {};

		// new //..//..//..
		uint8_t NAME[Const::PU_NAME_LEN] = {};			SecureWipe(NAME, Const::PU_NAME_LEN);
		uint8_t PASS[Const::PU_PASS_LEN] = {};			SecureWipe(PASS, Const::PU_PASS_LEN);
		uint8_t DESC[Const::PU_DESC_LEN] = {};			SecureWipe(DESC, Const::PU_DESC_LEN);

		auto fillROW = [&](size_t& local_OF, size_t isalt_size, uint8_t* TEXT, size_t TEXTsize, const char* text2cpy) {

			// SALT & COPY
			// ----------------------
			uint8_t* iSALT = KDF::RandomSalt(isalt_size);
			memcpy(DATA + local_OF, iSALT, isalt_size);			local_OF += isalt_size;

			// INSIDE HASH
			// ----------------------
			SecureWipe(INSIDE_HASH, Const::PU_HASH_SIZE);
			KDF::Harpid(MASTER_HASH, Const::MASTER_HASH_SIZE, iSALT, isalt_size, INSIDE_HASH, Const::PU_HASH_SIZE);

			SecureWipe(iSALT, isalt_size); delete[] iSALT; iSALT = nullptr;

			// TEXT + rand after '\0' & COPY
			// ----------------------
			SecureWipe(TEXT, TEXTsize);
			UTF8_to_NFC::to_nfc_bytes(text2cpy, TEXTsize + 1, TEXT, TEXTsize); // +1 for char* (see LwrsFileUnit.hpp -> struct PU)
			// rand after '\0'
			bool isStart = false;
			for (size_t i = 0; i < TEXTsize; ++i) {
				if (TEXT[i] == 0) {
					if (!isStart) {
						isStart = true;
					}
					else {
						TEXT[i] = KDF::RANDOM();
					}
				}
			}
			memcpy(DATA + local_OF, TEXT, TEXTsize);
			SecureWipe(TEXT, TEXTsize);

			// XOR Shuffle
			// ----------------------
			XOR_Shuffle::for_INSIDE_PU_IN(DATA + local_OF, TEXTsize, INSIDE_HASH, Const::PU_HASH_SIZE);
			local_OF += TEXTsize;

		};

		Core::DROPFRAME_ENCODE(DecodeStatus::I_XOR_Shuffle);
		for (size_t i = 0; i < PUcount; ++i) {

			size_t local_OF = OF + (i * SIZE_ONE_PU);
			PU unit = LwrsFileUnit::GetPUByfloatID(links[i]);

			fillROW(local_OF, Const::PU_NAME_SALT_SIZE, NAME, Const::PU_NAME_LEN, unit.name);
			fillROW(local_OF, Const::PU_PASS_SALT_SIZE, PASS, Const::PU_PASS_LEN, unit.pass);
			fillROW(local_OF, Const::PU_DESC_SALT_SIZE, DESC, Const::PU_DESC_LEN, unit.desc);

			unit.Clear();
		}
		SecureWipe(INSIDE_HASH, Const::PU_HASH_SIZE);
		
		// GLOBAL XOR
		// --------------------------------------------------------------------
		Core::DROPFRAME_ENCODE(DecodeStatus::G_XOR_Shuffle);
		XOR_Shuffle::for_GDATA_IN(DATA + OF, (PUcount * SIZE_ONE_PU), MASTER_HASH, Const::MASTER_HASH_SIZE);
		OF += (PUcount * SIZE_ONE_PU);
		
		// FILE HASH
		// --------------------------------------------------------------------
		ENDxxFHASH(OF);
		return DATA;
	}

};