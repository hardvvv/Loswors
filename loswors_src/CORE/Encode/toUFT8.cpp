#include "toUFT8.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif



    // 1. For const wchar_t* (UTF-16 from Windows API)
    std::string toUFT8::From(const wchar_t* wstr) {
        if (!wstr || *wstr == L'\0') return "";
#ifdef _WIN32
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        if (size <= 0) return "";
        std::string result(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &result[0], size, NULL, NULL);
        return result;
#else
        return ""; // On Linux wchar_t is almost never used
#endif
    }

    // 2. For std::wstring
    std::string toUFT8::From(const std::wstring& wstr) {
        return From(wstr.c_str());
    }

    // 3. For const char* (System ANSI / Windows-1251 from argv or tinyfiledialogs)
    std::string toUFT8::From(const char* str) {
        if (!str || *str == '\0') return "";
#ifdef _WIN32
        // First, convert from system Windows (ANSI/CP_ACP) to UTF-16
        int wsize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
        if (wsize <= 0) return str; // If fails, return as is
        std::wstring wbuf(wsize - 1, 0);
        MultiByteToWideChar(CP_ACP, 0, str, -1, &wbuf[0], wsize);

        // Then from UTF-16 to normal UTF-8
        return From(wbuf);
#else
        return std::string(str); // On Linux/macOS char* is already UTF-8
#endif
    }

    // 4. For std::string
    std::string toUFT8::From(const std::string& str) {
        return From(str.c_str());
    }

    // 5. For std::filesystem::path
    std::string toUFT8::From(const std::filesystem::path& path) {
#ifdef _WIN32
        return From(path.wstring()); // Path on Windows inside wchar_t
#else
        return path.string();
#endif
    }

    // Converter BACK to fs::path for opening files (ifstream / ofstream)
    std::filesystem::path toUFT8::ToPath(const std::string& utf8_str) {
        return std::filesystem::path(reinterpret_cast<const char8_t*>(utf8_str.c_str()));
    }