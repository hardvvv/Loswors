#include "FileSys.hpp"

#include <filesystem>
#include <fstream>

#include "../Encode/toUFT8.hpp" // for file work (fs::path)
#include "../Encode/UTF8_to_NFC.hpp"

namespace fs = std::filesystem;

// ---------
// For Cross platform Get work dir
// ---------
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <vector>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

namespace {
    // Cross platform Get work dir
    std::string get_executable_dir_utf8() {
        fs::path exe_dir;

#ifdef _WIN32
        // Windows: get a wide-string with a large buffer
        wchar_t buffer[32768];
        DWORD length = GetModuleFileNameW(NULL, buffer, 32768);
        if (length > 0 && length < 32768) {
            exe_dir = fs::path(buffer).parent_path();
        }
#elif defined(__linux__)
        // Linux: read symbolic link process
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count != -1) {
            exe_dir = fs::path(std::string(buffer, count)).parent_path();
        }
#elif defined(__APPLE__)
        // macOS: use _NSGetExecutablePath to get the executable path
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);
        std::vector<char> buffer(size);
        if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
            exe_dir = fs::canonical(buffer.data()).parent_path();
        }
#endif

        if (exe_dir.empty()) {
            exe_dir = fs::current_path();
        }

        // Return strictly in UTF-8 encoding
#if defined(__cpp_lib_char8_t) // C++20
        auto u8 = exe_dir.u8string();
        return std::string(u8.begin(), u8.end());
#else // C++17
        return exe_dir.u8string();
#endif
    }


    const std::string Data_DirName          = "Data";

    const std::string Localization_DirName  = "Languages";
}

namespace FileSys {
    
    //      ____       _   _        ____      _                 
    //     |  _ \ __ _| |_| |__    / ___| ___| |_ ___ _ __ ___  
    //     | |_) / _` | __| '_ \  | |  _ / _ \ __/ _ \ '__/ __| 
    //     |  __/ (_| | |_| | | | | |_| |  __/ ||  __/ |  \__ \ 
    //     |_|   \__,_|\__|_| |_|  \____|\___|\__\___|_|  |___/ 
    //  -------------------------------------------------------------------------

    std::string PATH_COMBINE_utf8(std::string ParentSegment_utf8, std::string ChildSegment_utf8) {
        if (ParentSegment_utf8.empty()) return ChildSegment_utf8;
        if (ChildSegment_utf8.empty()) return ParentSegment_utf8;

        // (Windows: '\', Linux/macOS: '/')
        constexpr char preferred_separator = static_cast<char>(fs::path::preferred_separator);

        // delete trailing in dir ('/' '\')
        while (!ParentSegment_utf8.empty() && (ParentSegment_utf8.back() == '/' || ParentSegment_utf8.back() == '\\')) {
            ParentSegment_utf8.pop_back();
        }

        // leading div in name
        size_t start_idx = 0;
        while (start_idx < ChildSegment_utf8.size() && (ChildSegment_utf8[start_idx] == '/' || ChildSegment_utf8[start_idx] == '\\')) {
            start_idx++;
        }

        return ParentSegment_utf8 + preferred_separator + ChildSegment_utf8.substr(start_idx);
    }

    std::string GetSettings_FP_utf8() {
        std::string ret = get_executable_dir_utf8();
        ret = PATH_COMBINE_utf8(ret, Data_DirName);
        ret = PATH_COMBINE_utf8(ret, "settings.txt");
        return ret;
    }
    std::string GetMonoProg_FP_utf8() {
        std::string ret = get_executable_dir_utf8();
        ret = PATH_COMBINE_utf8(ret, Data_DirName);
        ret = PATH_COMBINE_utf8(ret, "monoprog.ping");
        return ret;
    }

    std::string GetLicenses_FP_utf8(std::string FILE_NAME_utf8) {
        std::string ret = get_executable_dir_utf8();
        ret = PATH_COMBINE_utf8(ret, "licenses");
        ret = PATH_COMBINE_utf8(ret, FILE_NAME_utf8);
        return ret;
    }
    std::string GetLicense_FP_utf8() {
        std::string ret = get_executable_dir_utf8();
        ret = PATH_COMBINE_utf8(ret, "LICENSE");
        return ret;
    }

    std::string GetLocalization_DP_utf8() {
        std::string ret = get_executable_dir_utf8();
        ret = PATH_COMBINE_utf8(ret, Data_DirName);
        ret = PATH_COMBINE_utf8(ret, Localization_DirName);
        return ret;
    }
    std::string GetLocalization_FP_utf8(std::string FILE_NAME_utf8) {
        std::string ret = get_executable_dir_utf8();
        ret = PATH_COMBINE_utf8(ret, Data_DirName);
        ret = PATH_COMBINE_utf8(ret, Localization_DirName);
        ret = PATH_COMBINE_utf8(ret, FILE_NAME_utf8);
        return ret;
    }

    //      _____ _ _       __        __         _    
    //     |  ___(_) | ___  \ \      / /__  _ __| | __
    //     | |_  | | |/ _ \  \ \ /\ / / _ \| '__| |/ /
    //     |  _| | | |  __/   \ V  V / (_) | |  |   < 
    //     |_|   |_|_|\___|    \_/\_/ \___/|_|  |_|\_\
    //  -------------------------------------------------------------------------

    const char* GetStatusTEXTen(Status status) {
        switch (status)
        {
        case Status::Success:
            return "Success";
        case Status::NotFound:
            return "Not Found";
        case Status::IsDirectory:
            return "Directory!";
        case Status::InvalidPath:
            return "Invalid Path";
        case Status::AccessDenied:
            return "Access Denied";
        case Status::FileLocked:
            return "File Locked";
        case Status::DirectoryCreationFailed:
            return "Fail Create Dir";
        case Status::FailedToOpen:
            return "Open Fail";
        case Status::ReadError:
            return "Read Error";
        case Status::WriteError:
            return "Write Error";
        case Status::FileIsEmpty:
            return "Empty File";
        case Status::InvalidEncoding:
            return "Invalid Encoding";
        default:
            break;
        }
        return "-1";
    }

    Status delete_file(const std::string& FP_utf8) {
        if (FP_utf8.empty()) return Status::InvalidPath;

        std::error_code ec;

        fs::path file_path = toUFT8::ToPath(FP_utf8).lexically_normal();

        if (!fs::exists(file_path, ec)) return Status::NotFound;
        if (fs::is_directory(file_path, ec)) return Status::IsDirectory;

        bool isDEL = fs::remove(file_path, ec);
        if (ec) {
            if (ec == std::errc::permission_denied)         return Status::AccessDenied;
            if (ec == std::errc::device_or_resource_busy)   return Status::FileLocked;

            return Status::FailedToOpen;
        }

        if (!isDEL) return Status::NotFound;

        return Status::Success;
    }
    Status rename_file(const std::string& fromFP_utf8, const std::string& toFP_utf8) {
        if (fromFP_utf8.empty() || toFP_utf8.empty()) return Status::InvalidPath;

        std::error_code ec;

        fs::path old_path = toUFT8::ToPath(fromFP_utf8).lexically_normal();
        fs::path new_path = toUFT8::ToPath(toFP_utf8).lexically_normal();

        // Check if the original file exists
        if (!fs::exists(old_path, ec)) return Status::NotFound;
        if (fs::is_directory(old_path, ec)) return Status::IsDirectory;

        // Check if the target directory exists (to prevent moving to a non-existent directory)
        if (!fs::exists(new_path.parent_path(), ec)) return Status::InvalidPath;

        // Rename (works as moving to another path)
        fs::rename(old_path, new_path, ec);

        if (ec) {
            if (ec == std::errc::permission_denied)         return Status::AccessDenied;
            if (ec == std::errc::device_or_resource_busy)   return Status::FileLocked;
            if (ec == std::errc::file_exists)               return Status::AccessDenied; // If the target file is blocked or occupied
            if (ec == std::errc::cross_device_link)         return Status::FailedToOpen; // If attempting to move between different drives (C:\ -> D:\)

            return Status::FailedToOpen;
        }

        return Status::Success;
    }

    Status read_text_lines_from_file(std::vector<std::string>& line_arr, const std::string& FP_utf8) {
        if (FP_utf8.empty()) return Status::InvalidPath;

        fs::path path_str = toUFT8::ToPath(FP_utf8).lexically_normal();

        // open  (C++17/20)
        std::ifstream file(path_str);

        if (!file.is_open()) return Status::FailedToOpen;

        std::string line;
        while (std::getline(file, line)) {
            // delete Windows CR ('\r') for claer UTF-8
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            line_arr.push_back(line);
        }

        return Status::Success;
    }
    Status write_text_to_file(const std::string& FP_utf8, const std::string& text) {
        if (FP_utf8.empty()) return Status::InvalidPath;

        fs::path file_path = toUFT8::ToPath(FP_utf8).lexically_normal();

        // dir create
        std::error_code ec;
        if (file_path.has_parent_path()) {
            fs::create_directories(file_path.parent_path(), ec);
            if (ec) return Status::DirectoryCreationFailed;
        }

        std::ofstream file;
        file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

        try {
            file.open(file_path, std::ios::out);

            file << text;

            file.flush();
            file.close();

            return Status::Success;
        }
        catch (const std::ios_base::failure&) {
            return Status::WriteError;
        }
        catch (const std::exception&) {
            return Status::WriteError;
        }
    }

    Status read_bytes_from_file(const std::string& FP_utf8, uint8_t*& data, size_t& NewSize) {
        NewSize = 0;
        data = nullptr;

        if (FP_utf8.empty()) return Status::InvalidPath;

        std::error_code ec;
        fs::path file_path = toUFT8::ToPath(FP_utf8).lexically_normal();

        if (!fs::exists(file_path, ec))         return Status::NotFound;
        if (fs::is_directory(file_path, ec))    return Status::IsDirectory;

        auto file_size = fs::file_size(file_path, ec);
        if (ec) return Status::ReadError;
        if (file_size == 0) return Status::FileIsEmpty;

        data = new (std::nothrow) uint8_t[file_size];
        if (!data) return Status::ReadError;

        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            file.open(file_path, std::ios::in | std::ios::binary);

            file.read(reinterpret_cast<char*>(data), file_size);
            file.close();

            NewSize = static_cast<size_t>(file_size);
            return Status::Success;
        }
        catch (...) {
            delete[] data;
            data = nullptr;
            NewSize = 0;

            return Status::ReadError;
        }

        return Status::Success;
    }
    Status write_bytes_to_file(const std::string& FP_utf8, const uint8_t* data, size_t size) {
        if (FP_utf8.empty()) return Status::InvalidPath;
        if (!data && size > 0) return Status::WriteError;

        fs::path target_path = toUFT8::ToPath(FP_utf8).lexically_normal();

        std::error_code ec;

        // dir create
        if (target_path.has_parent_path()) {
            fs::path parent_dir = target_path.parent_path();
            if (!parent_dir.empty() && !fs::exists(parent_dir, ec)) {
                fs::create_directories(parent_dir, ec);
                if (ec) return Status::DirectoryCreationFailed;
            }
        }

        std::ofstream file(target_path, std::ios::out | std::ios::binary);
        if (!file.is_open()) return Status::FailedToOpen;

        if (size > 0) {
            file.write(reinterpret_cast<const char*>(data), size);
        }

        file.flush();

        if (file.fail() || file.bad()) {
            file.close();
            return Status::WriteError;
        }

        file.close();
        return Status::Success;
    }

    Status get_all_files_in_dir(const std::string& DP_utf8, std::vector<std::string>& FileArr, const std::string& type) {
        if (DP_utf8.empty()) return Status::InvalidPath;

        std::error_code ec;

        fs::path dir_path = toUFT8::ToPath(DP_utf8).lexically_normal();

        if (!fs::exists(dir_path, ec))          return Status::NotFound;
        if (!fs::is_directory(dir_path, ec))    return Status::IsDirectory;

        // Normalize file extension filter
        std::string ext = type;
        if (!ext.empty() && ext.front() != '.') {
            ext = "." + ext;
        }

        // dir iterator
        fs::directory_iterator it(dir_path, ec);
        if (ec) {
            if (ec == std::errc::permission_denied) return Status::AccessDenied;
            return Status::FailedToOpen;
        }

        fs::directory_iterator end;

        // scan
        while (it != end) {
            const auto& entry = *it;

            if (entry.is_regular_file(ec)) {
                if (ext.empty() || entry.path().extension() == ext) {

                    // UTF-8 C++20/MSVC:
                    auto u8_path = entry.path().filename().u8string();
                    std::string u8_filename(u8_path.begin(), u8_path.end());

                    const uint8_t* raw_bytes = reinterpret_cast<const uint8_t*>(u8_filename.data());
                    size_t raw_size = u8_filename.size();

                    char clean_buf[512] = { 0 };

                    // NFC norm
                    if (UTF8_to_NFC::to_clean_string(raw_bytes, raw_size, clean_buf, sizeof(clean_buf))) {
                        FileArr.push_back(clean_buf);
                    }
                    else {
                        FileArr.push_back(u8_filename);
                    }
                }
            }

            it.increment(ec);
            if (ec) {
                if (ec == std::errc::permission_denied) return Status::AccessDenied;
                return Status::ReadError;
            }
        }

        return Status::Success;
    }

    //      ____  _             
    //     |  _ \(_)_ __   __ _ 
    //     | |_) | | '_ \ / _` |
    //     |  __/| | | | | (_| |
    //     |_|   |_|_| |_|\__, |
    // ------------------ |___/ -------------------------------------------------

    bool Ping_FileExists(const std::string& FP_utf8) noexcept {
        if (FP_utf8.empty()) return false;

        fs::path pathcontext = toUFT8::ToPath(FP_utf8);

        std::error_code ec;
        return fs::is_regular_file(pathcontext, ec);
    }
    bool Ping_DirExists(const std::string& DP_utf8) noexcept {
        if (DP_utf8.empty()) return false;

        fs::path pathcontext = toUFT8::ToPath(DP_utf8);

        std::error_code ec;
        return fs::is_directory(pathcontext, ec);
    }

}