#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

inline void SecureWipe(uint8_t* data, size_t len) {
    if (!data) return;
    volatile uint8_t* p = data;
    while (len--) *p++ = 0;
}
inline void SecureWipe(char* data, size_t len) {
    SecureWipe(reinterpret_cast<uint8_t*>(data), len);
}

inline void SecureWipe(std::string& str) {
    if (!str.empty()) {
        SecureWipe(reinterpret_cast<uint8_t*>(str.data()), str.size());
        str.clear();
    }
}