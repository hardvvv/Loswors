#pragma once

#include <string>
#include <filesystem>

// A unified function to convert everything to UTF-8
class toUFT8 {
public:
    toUFT8() = delete;
    // 1. For const wchar_t* (UTF-16 from Windows API)
    static std::string From(const wchar_t* wstr);

    // 2. For std::wstring
    static std::string From(const std::wstring& wstr);

    // 3. For const char* (System ANSI / Windows-1251 from argv or tinyfiledialogs)
    static std::string From(const char* str);

    // 4. For std::string
    static std::string From(const std::string& str);

    // 5. For std::filesystem::path
    static std::string From(const std::filesystem::path& path);

    // Converter BACK to fs::path for opening files (ifstream / ofstream)
    static std::filesystem::path ToPath(const std::string& utf8_str);
};