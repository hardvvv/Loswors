/*

    This namespace - API connection between file system and program
    Inside the program, all strings are UTF8, in and out
    std::filesystem::path is used to interact with out system

*/

#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace FileSys {

    //      ____       _   _        ____      _                 
    //     |  _ \ __ _| |_| |__    / ___| ___| |_ ___ _ __ ___  
    //     | |_) / _` | __| '_ \  | |  _ / _ \ __/ _ \ '__/ __| 
    //     |  __/ (_| | |_| | | | | |_| |  __/ ||  __/ |  \__ \ 
    //     |_|   \__,_|\__|_| |_|  \____|\___|\__\___|_|  |___/ 
    //  -------------------------------------------------------------------------
    // FP - (file path)  |  DP - (dir path)

    std::string PATH_COMBINE_utf8(std::string ParentSegment_utf8, std::string ChildSegment_utf8);

    std::string GetSettings_FP_utf8();
    std::string GetMonoProg_FP_utf8();

    std::string GetLicenses_FP_utf8(std::string FILE_NAME_utf8);
    std::string GetLicense_FP_utf8();

    std::string GetLocalization_DP_utf8();
    std::string GetLocalization_FP_utf8(std::string FILE_NAME_utf8);

    //      _____ _ _       __        __         _    
    //     |  ___(_) | ___  \ \      / /__  _ __| | __
    //     | |_  | | |/ _ \  \ \ /\ / / _ \| '__| |/ /
    //     |  _| | | |  __/   \ V  V / (_) | |  |   < 
    //     |_|   |_|_|\___|    \_/\_/ \___/|_|  |_|\_\
    //  -------------------------------------------------------------------------

	enum class Status : uint8_t {
	    Success = 0,

	    NotFound = 1,                   // File not found
	    IsDirectory = 2,                // Path leads to a directory, not a file
	    InvalidPath = 3,                // Invalid characters in path or path too long

	    AccessDenied = 4,               // No permissions to read/write (Permission denied)
	    FileLocked = 5,                 // File is locked by another process

	    DirectoryCreationFailed = 6,    // Failed to create the parent directory
	    FailedToOpen = 7,               // Failed to open (general `is_open()` error)
	    ReadError = 8,                  // Read error (corrupted stream / hardware failure)
	    WriteError = 9,                 // Write error (e.g., disk space is full)

	    FileIsEmpty = 10,                // File is empty
	    InvalidEncoding = 11             // Encoding error (e.g., not UTF-8 / invalid text)
    };
    const char* GetStatusTEXTen(Status status);

    Status delete_file(const std::string& FP_utf8);
    Status rename_file(const std::string& fromFP_utf8, const std::string& toFP_utf8);

    Status read_text_lines_from_file(std::vector<std::string>& line_arr, const std::string& FP_utf8);
    Status write_text_to_file(const std::string& FP_utf8, const std::string& text);              // if not create if exist rewrite

    Status read_bytes_from_file(const std::string& FP_utf8, uint8_t*& data, size_t& NewSize);
    Status write_bytes_to_file(const std::string& FP_utf8, const uint8_t* data, size_t size);    // if not create if exist rewrite

    Status get_all_files_in_dir(const std::string& DP_utf8, std::vector<std::string>& FileArr, const std::string& type); // .type are include

    //      ____  _             
    //     |  _ \(_)_ __   __ _ 
    //     | |_) | | '_ \ / _` |
    //     |  __/| | | | | (_| |
    //     |_|   |_|_| |_|\__, |
    // ------------------ |___/ -------------------------------------------------

    bool Ping_FileExists(const std::string& FP_utf8) noexcept;
    bool Ping_DirExists(const std::string& DP_utf8) noexcept;

}