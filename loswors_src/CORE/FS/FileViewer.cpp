#include "FileViewer.hpp"
#include <vector>
#include <string>
#include <cstring>
#include "tinyfiledialogs.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace {
    // Safe conversion from UTF-8 to UTF-16
    std::wstring Utf8ToUtf16(const char* str) {
        if (!str || *str == '\0') return L"";
        int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
        if (len <= 0) return L"";

        std::wstring wstr(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], len);
        if (!wstr.empty() && wstr.back() == L'\0') {
            wstr.pop_back();
        }
        return wstr;
    }

    // Safe conversion UTF-16 -> UTF-8
    void Utf16ToUtf8(const wchar_t* wstr, char* outBuffer, int maxLen) {
        if (!outBuffer || maxLen <= 0) return;
        if (!wstr) {
            outBuffer[0] = '\0';
            return;
        }
        std::memset(outBuffer, 0, maxLen);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, outBuffer, maxLen - 1, NULL, NULL);
    }
#endif

    size_t SafeStrLen(const char* str, size_t maxLen) {
        if (!str) return 0;
        size_t len = 0;
        while (len < maxLen && str[len] != '\0') {
            len++;
        }
        return len;
    }
}


namespace FileViewer {

    

    bool OpenFile(
        char* outPath,
        int maxLen,
        const char* title,
        const std::vector<std::string>& filterPatterns,
        const char* filterDescription
    ) {
        if (!outPath || maxLen <= 0) return false;

        size_t currentLen = SafeStrLen(outPath, static_cast<size_t>(maxLen));
        const char* initialPath = (currentLen > 0 && currentLen < static_cast<size_t>(maxLen)) ? outPath : nullptr;

#ifdef _WIN32
        // --- Windows (Using Wide-versions of tinyfiledialogs) ---
        std::wstring wTitle = Utf8ToUtf16(title ? title : "Select file:");
        std::wstring wInitialPath = Utf8ToUtf16(initialPath);
        std::wstring wFilterDesc = Utf8ToUtf16(filterDescription);

        std::vector<std::wstring> wPatterns;
        std::vector<const wchar_t*> wFilterPtrs;
        wPatterns.reserve(filterPatterns.size());
        wFilterPtrs.reserve(filterPatterns.size());

        for (const auto& pattern : filterPatterns) {
            wPatterns.push_back(Utf8ToUtf16(pattern.c_str()));
            wFilterPtrs.push_back(wPatterns.back().c_str());
        }

        int numFilters = static_cast<int>(wFilterPtrs.size());
        const wchar_t* const* filters = numFilters > 0 ? wFilterPtrs.data() : nullptr;

        const wchar_t* resultW = tinyfd_openFileDialogW(
            wTitle.c_str(),
            wInitialPath.empty() ? nullptr : wInitialPath.c_str(),
            numFilters,
            filters,
            wFilterDesc.empty() ? nullptr : wFilterDesc.c_str(),
            0
        );

        if (resultW != nullptr) {
            Utf16ToUtf8(resultW, outPath, maxLen);
            return true;
        }

#else
        // --- Linux / macOS ---
        std::vector<const char*> filterPtrs;
        filterPtrs.reserve(filterPatterns.size());
        for (const auto& pattern : filterPatterns) {
            filterPtrs.push_back(pattern.c_str());
        }

        int numFilters = static_cast<int>(filterPtrs.size());
        const char* const* filters = numFilters > 0 ? filterPtrs.data() : nullptr;

        const char* result = tinyfd_openFileDialog(
            title ? title : "Select file:",
            initialPath,
            numFilters,
            filters,
            filterDescription,
            0
        );

        if (result != nullptr) {
            std::memset(outPath, 0, maxLen);
            std::strncpy(outPath, result, maxLen - 1);
            outPath[maxLen - 1] = '\0';
            return true;
        }
#endif

        return false;
    }

    bool OpenFolder(
        char* outPath,
        int maxLen,
        const char* title
    ) {
        if (!outPath || maxLen <= 0) return false;

        size_t currentLen = SafeStrLen(outPath, static_cast<size_t>(maxLen));
        const char* initialPath = (currentLen > 0 && currentLen < static_cast<size_t>(maxLen)) ? outPath : nullptr;

#ifdef _WIN32
        // --- Windows ---
        std::wstring wTitle = Utf8ToUtf16(title ? title : "Select folder:");
        std::wstring wInitialPath = Utf8ToUtf16(initialPath);

        const wchar_t* resultW = tinyfd_selectFolderDialogW(
            wTitle.c_str(),
            wInitialPath.empty() ? nullptr : wInitialPath.c_str()
        );

        if (resultW != nullptr) {
            Utf16ToUtf8(resultW, outPath, maxLen);
            return true;
        }

#else
        // --- Linux / macOS ---
        const char* result = tinyfd_selectFolderDialog(
            title ? title : "Select folder:",
            initialPath
        );

        if (result != nullptr) {
            std::memset(outPath, 0, maxLen);
            std::strncpy(outPath, result, maxLen - 1);
            outPath[maxLen - 1] = '\0';
            return true;
        }
#endif

        return false;
    }
}