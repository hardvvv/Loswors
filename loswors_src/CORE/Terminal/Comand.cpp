#include "Comand.hpp"
#include <stdexcept>
#include <cstdlib> // for system()


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#include <shellapi.h>
#undef near
#undef far
#endif

void OpenUrlSafe(const char* url) {
#if defined(_WIN32)
    // Windows
    ShellExecuteA(NULL, "open", url, NULL, NULL, 1); // 1 = SW_SHOWNORMAL

#elif defined(__APPLE__)
    //  macOS
    std::string cmd = std::string("open \"") + url + "\"";
    std::system(cmd.c_str());

#elif defined(__linux__)
    //  Linux
    std::string cmd = std::string("xdg-open \"") + url + "\"";
    std::system(cmd.c_str());

#else
    throw std::runtime_error("OpenUrlSafe is not implemented for this OS.");
#endif
}