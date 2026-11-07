#include "MonoProg.hpp"

#include "../Const.hpp"
#include "../Settings.hpp"
#include "../Log.hpp"
#include "../lwrs/Encryption/KDF.hpp"
#include "../Timer/timer.hpp"
#include "../FS/FileSys.hpp"

#include <cstdint>
#include <string>
#include <format>

#include <iostream>

namespace {
    namespace ltxt {
        // FT = format text
        constexpr char Create_FT[]      = "[ MonoProg ] >> Ping File create status: {}";
        constexpr char LoadPath_FT[]    = "[ MonoProg ] >> (From 2nd process) Load Path: {}";
        constexpr char EndInit[]        = "[ MonoProg ] >> End INIT <<";
    }
}

namespace {

	uint64_t UNIQ_ID = 0;

    timer t_afterstart = timer();   // need to fast check monoprog file after start
    timer t_killself = timer();

    inline void CreatePingFile(const char* path) {

        size_t path_len = (path != nullptr) ? strlen(path) : 0;
        size_t DATAsz = sizeof(UNIQ_ID) + path_len;
        std::vector<uint8_t> DATA(DATAsz);

        memcpy(DATA.data(), &UNIQ_ID, sizeof(UNIQ_ID));

        if (path_len > 0) {
            memcpy(DATA.data() + sizeof(UNIQ_ID), path, path_len);
        }

        Log::add(std::format(ltxt::Create_FT, FileSys::GetStatusTEXTen(
            FileSys::write_bytes_to_file(FileSys::GetMonoProg_FP_utf8(), DATA.data(), DATA.size())
        )), L_OK);
    }


    inline bool IsNeedToKillSelf_NOPARSE() {

        uint8_t* DATA = nullptr; size_t DATAsz = 0;
        FileSys::Status st = FileSys::read_bytes_from_file(FileSys::GetMonoProg_FP_utf8(), DATA, DATAsz);

        auto clearBeforReturn = [&]() {
            delete[] DATA; DATAsz = 0; DATA = nullptr;
        };

        if (st == FileSys::Status::Success) {
            if (DATAsz >= sizeof(uint64_t) && DATA) {

                uint64_t GET_UNIQ_ID = 0;
                memcpy(&GET_UNIQ_ID, DATA, sizeof(GET_UNIQ_ID));

                clearBeforReturn();
                return (GET_UNIQ_ID != UNIQ_ID);
            }
        }
        else if (st == FileSys::Status::NotFound) {
            CreatePingFile("\0");
        }

        clearBeforReturn();
        return false;
    }

    inline bool IsNeedToKillSelf_PARSE() {

        uint8_t* DATA = nullptr; size_t DATAsz = 0;
        FileSys::Status st = FileSys::read_bytes_from_file(FileSys::GetMonoProg_FP_utf8(), DATA, DATAsz);

        auto clearBeforReturn = [&]() {
            delete[] DATA; DATAsz = 0; DATA = nullptr;
        };

        if (st == FileSys::Status::Success) {
            if (DATAsz >= sizeof(uint64_t) && DATA) {

                uint64_t GET_UNIQ_ID = 0;
                char GET_PATH[Const::MAX_PATH_LEN + 1] = { 0 };

                memcpy(&GET_UNIQ_ID, DATA, sizeof(GET_UNIQ_ID));

                size_t cpySZ = std::min(DATAsz - sizeof(GET_UNIQ_ID), sizeof(GET_PATH) - 1);
                if (cpySZ > 0) memcpy(GET_PATH, DATA + sizeof(GET_UNIQ_ID), cpySZ);
                GET_PATH[cpySZ] = '\0';

                if (GET_UNIQ_ID != UNIQ_ID) {
                    UNIQ_ID = GET_UNIQ_ID;
                    CreatePingFile(Settings::path_f0);

                    clearBeforReturn();
                    return true;
                }

                // ADD File To Program
                if (GET_PATH[0] != '\0' && FileSys::Ping_FileExists(GET_PATH)) {
                    if (!Settings::is_2file) {
                        Settings::is_2file = true;

                        std::string f0 = Settings::path_f0;
                        std::string f1 = GET_PATH;
                        if (f0.ends_with(std::format("{}{}", "_1.", Const::LOSWORS_FILE_FORMAT)) ||
                            f1.ends_with(std::format("{}{}", "_0.", Const::LOSWORS_FILE_FORMAT))) {
                            std::swap(f0, f1);
                        }
                        Settings::ClearFPath0();
                        Settings::ClearFPath1();
                        snprintf(Settings::path_f0, sizeof(Settings::path_f0), "%s", f0.c_str());
                        snprintf(Settings::path_f1, sizeof(Settings::path_f1), "%s", f1.c_str());

                        Log::add(std::format(ltxt::LoadPath_FT, GET_PATH), L_INFO);
                        CreatePingFile("\0");
                    }   
                }
            }
        }
        else if (st == FileSys::Status::NotFound) {
            CreatePingFile("\0");
        }

        clearBeforReturn();
        return false;
    }

}

void MonoProg::_INIT_() {
    UNIQ_ID = 0;
    for (size_t i = 0; i < sizeof(UNIQ_ID); ++i) {
        UNIQ_ID = (UNIQ_ID << 8) | KDF::RANDOM();
    }

    t_afterstart.Start();
    t_killself.Start();

    CreatePingFile("\0");

	Log::add(ltxt::EndInit, L_INFO);
}


bool MonoProg::IsKillSelf() {

    if (t_afterstart.GetTime_ns() < Const::TIME_AFTERSTART_TO_PROCESS_DATA_ns) {
        // fast ping
        if (t_killself.GetTime_ns() > Const::TIME_FAST_MONOPING_ns) {
            t_killself.Start();
            return IsNeedToKillSelf_PARSE();
        }
    }
    else {
        if (t_killself.GetTime_ns() > Const::TIME_NORMAL_MONOPING_ns) {
            t_killself.Start();
            return IsNeedToKillSelf_NOPARSE();
        }
    }

    return false;
}