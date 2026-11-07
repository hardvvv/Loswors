#include "LocalSys.hpp"

#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <format>

#include "loc.hpp"

class LocEditor {
public:
    static void SetV(const std::string& text, int id) {
        loc::arr[id]->SetVal(text);
    }
};

#include "../CORE/Log.hpp"
#include "../CORE/FS/FileSys.hpp"
#include "../CORE/Settings.hpp"


// LANG`s
#include "languages/en_loc.hpp"
#include "languages/ru_loc.hpp"

namespace {
    namespace ltxt {
        constexpr char CreateFileDone_FT[]  = "[ Localiz Sys ] >> Localiz. file ({}) create Done!";
        constexpr char ErrCreateFile_2FT[]  = "[ Localiz Sys ] >> Error ({}) while create localiz. file ({})";
        constexpr char ErrReadFile_2FT[]    = "[ Localiz Sys ] >> Error ({}) while reading file ({})";
        constexpr char WarningEmpty_FT[]    = "[ Localiz Sys ] >> WARNING -> LocalSys file ({}) is empty!";
        constexpr char OkLoad_FT[]          = "[ Localiz Sys ] >> Load ({}) localiz. Done!";
        constexpr char Select_FT[]          = "[ Localiz Sys ] >> Localiz. - ({}) - selected";
        constexpr char GetFileArrErr_FT[]   = "[ Localiz Sys ] >> Error get arr of all localiz. files - ({})";
    }
}

constexpr char LOC_FILE_EXTENSION[] = ".txt";

namespace {

    char CurLocName[Const::MAX_LANGUAGE_NAME + 1] = "en";
    std::string CurCodepointRange = "0x0020-0x007F"; // en

    std::string preqalc_AllLangName = "en"; // --------------------------------

    std::vector<std::string> all_loc_file;
    int AciveLocID = -1;

    void ClearExtention(std::string& text) {
        std::string_view sv = text;
        size_t exten_size = sizeof(LOC_FILE_EXTENSION) - 1;
        if (sv.size() >= exten_size && sv.substr(sv.size() - exten_size) == LOC_FILE_EXTENSION) {
            sv.remove_suffix(exten_size);
        }
        text = sv;
    }

    inline void _preqacl_AllLangName() {
        if (all_loc_file.size() != 0) preqalc_AllLangName.clear();

        for (size_t i = 0; i < all_loc_file.size(); ++i) {
            if (i > 0) {
                preqalc_AllLangName += ';';
            }
            std::string pp = all_loc_file[i];
            ClearExtention(pp);
            preqalc_AllLangName += pp;
        }
    }

    int _getlocID(const std::vector<std::string> allfile, const char* search) {
        if (!search || allfile.empty()) {
            return -1;
        }

        std::string_view target(search);

        for (size_t i = 0; i < allfile.size(); ++i) {
            std::string_view filename(allfile[i]);

            size_t dot_pos = filename.find('.');

            std::string_view name_before_dot = (dot_pos != std::string_view::npos)
                ? filename.substr(0, dot_pos)
                : filename;

            if (name_before_dot == target) {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    // from mem to fs
    inline void IfNotLoad() {

        auto Loadfff = [](const char* name, const unsigned char* fdata, size_t datalen) {
            std::string filename = std::format("{}{}", name, LOC_FILE_EXTENSION);
            std::string path = FileSys::GetLocalization_FP_utf8(filename);
            if (!FileSys::Ping_FileExists(path)) {
                FileSys::Status status = FileSys::write_bytes_to_file(path, fdata, datalen);
                if (status == FileSys::Status::Success) {
                    // log ok
                    Log::add(std::format(ltxt::CreateFileDone_FT, name), L_OK);
                }
                else {
                    // log not ok
                    Log::add(std::format(ltxt::ErrCreateFile_2FT, FileSys::GetStatusTEXTen(status), name), L_ERROR);
                }
            }
        };

        // | Add more .hpp for more hardcoded localization
        // |__________________________________________

        Loadfff("en", en_loc, en_loc_len);
        Loadfff("ru", ru_loc, ru_loc_len);
    }

    // param(fname) need to by whith .txt or any extention .....
    bool LOAD_LOC_FROM_FILE(const std::string& fname) {
        std::vector<std::string> textlines;
        FileSys::Status st = FileSys::read_text_lines_from_file(textlines, FileSys::GetLocalization_FP_utf8(fname));
        if (st != FileSys::Status::Success) {
            Log::add(std::format(ltxt::ErrReadFile_2FT, FileSys::GetStatusTEXTen(st), fname), L_ERROR);
            return false;
        }
        if (textlines.empty()) {
            Log::add(std::format(ltxt::WarningEmpty_FT, fname), L_WARNING);
            return false;
        }

        CurCodepointRange = textlines[0];
        if (!CurCodepointRange.empty() && CurCodepointRange.back() == '\r') {
            CurCodepointRange.pop_back();
        }

        auto addTO = [](const std::string& key, const std::string& text) {
            for (int i = 0; i < loc::size; ++i) {
                if (loc::arr[i]->IsCompared(key.c_str())) {
                    LocEditor::SetV(text, i);
                    return;
                }
            }

            // FOR debug
            // std::cout << "Loc Point not found: " << key << std::endl;
        };

        std::string current_key = "";
        std::string current_text = "";

        bool inside_key_block = false;

        for (size_t i = 1; i < textlines.size(); ++i) {
            std::string line = textlines[i];

            if (!line.empty() && line.back() == '\r') line.pop_back();

            // if start "</"
            if (line.rfind("</", 0) == 0) {
                
                if (inside_key_block) {
                    addTO(current_key, current_text);
                    current_text.clear();
                }

                // ignore "</"
                size_t pos = 2;

                // go to first REAL symbol
                while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
                    ++pos;
                }

                size_t key_start = pos;
                while (pos < line.size() && !std::isspace(static_cast<unsigned char>(line[pos]))) {
                    ++pos;
                }

                current_key = line.substr(key_start, pos - key_start);
                inside_key_block = true;
            }
            else if (inside_key_block) {
                if (!current_text.empty()) {
                    current_text += "\n";
                }
                current_text += line;
            }
        }

        if (inside_key_block) {
            addTO(current_key, current_text);
        }

        return true;
    }
};

void LocalSys::LoadLangByID(int id) {
    if (id < 0 || id >= all_loc_file.size()) return;

    auto write2CurLocName = [&]() {
        std::string fNOext = all_loc_file[id];
        ClearExtention(fNOext);

        size_t copy_len = std::min(fNOext.size(), sizeof(CurLocName) - 1);
        std::memcpy(CurLocName, fNOext.data(), copy_len);
        CurLocName[copy_len] = '\0';
    };

    if (LOAD_LOC_FROM_FILE(all_loc_file[id])) {

        AciveLocID = id;
        write2CurLocName();

        Log::add(std::format(ltxt::Select_FT, GetCurLangName()), L_INFO);
        Log::add(std::format(ltxt::OkLoad_FT, GetCurLangName()), L_INFO);
    }
}
int LocalSys::GetCurLangID() {
    return AciveLocID;
}

const char* LocalSys::GetCurLangName() {
    return CurLocName;
}
const char* LocalSys::GetAllLangName_forui() {
    return preqalc_AllLangName.c_str();
}
int LocalSys::GetLangCount() {
    return static_cast<int>(all_loc_file.size());
}


std::vector<int> LocalSys::GetCodePointARR() {
    std::set<int> codepoints_set;

    // ASCII (0x0020 .. 0x007F) default
    for (int i = 0x0020; i <= 0x007F; ++i) {
        codepoints_set.insert(i);
    }
    // CYRILLIC (0x0400 .. 0x04FF) optional (to draw the file path text)...
    for (int i = 0x0400; i <= 0x04FF; ++i) {
        codepoints_set.insert(i);
    }

    std::stringstream ss(CurCodepointRange);
    std::string token;

    while (ss >> token) {
        std::vector<std::string> parts;
        std::stringstream token_ss(token);
        std::string part;

        while (std::getline(token_ss, part, '-')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }

        // single symbol like 0x0401
        if (parts.size() == 1) {
            try {
                unsigned long code = std::stoul(parts[0], nullptr, 16);
                codepoints_set.insert(static_cast<int>(code));
            }
            catch (...) {}
        }
        // 0x0410-0x044F or 0x0010-0x0020-0x0030
        else if (parts.size() >= 2) {
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                try {
                    unsigned long start_code = std::stoul(parts[i], nullptr, 16);
                    unsigned long end_code = std::stoul(parts[i + 1], nullptr, 16);

                    if (start_code > end_code) {
                        std::swap(start_code, end_code);
                    }

                    for (unsigned long code = start_code; code <= end_code; ++code) {
                        codepoints_set.insert(static_cast<int>(code));
                    }
                }
                catch (...) {}
            }
        }
    }

    return std::vector<int>(codepoints_set.begin(), codepoints_set.end());
}


void LocalSys::_INIT_() {

    IfNotLoad();

    all_loc_file.clear(); all_loc_file.shrink_to_fit();
    FileSys::Status stat = FileSys::get_all_files_in_dir(FileSys::GetLocalization_DP_utf8(), all_loc_file, LOC_FILE_EXTENSION);
    if (stat == FileSys::Status::Success) {
        AciveLocID = _getlocID(all_loc_file, Settings::language);
        _preqacl_AllLangName();
        LoadLangByID(AciveLocID);
    }
    else {
        Log::add(std::format(ltxt::GetFileArrErr_FT, FileSys::GetStatusTEXTen(stat)), L_ERROR);
    }
}
