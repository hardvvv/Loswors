#include "UTF8_to_NFC.hpp"

#include <cstddef>
#include <cstring>
#include <cstdlib>
#include "../../External/utf8proc-2.11.3/utf8proc.h"


bool UTF8_to_NFC::to_nfc_bytes(const char* input_str, size_t input_size_bytes,
    uint8_t* out_buf, size_t buffer_size)
{
    if (!input_str || input_size_bytes == 0 || !out_buf || buffer_size == 0) {
        if (out_buf && buffer_size > 0) {
            std::memset(out_buf, 0, buffer_size);
        }
        return false;
    }

    // 1. Find the actual end of the input string, without going past input_size_bytes
    size_t actual_input_len = 0;
    while (actual_input_len < input_size_bytes && input_str[actual_input_len] != '\0') {
        actual_input_len++;
    }

    // If the input slice is empty — fill the output with zeros and exit
    if (actual_input_len == 0) {
        std::memset(out_buf, 0, buffer_size);
        return true;
    }

    // 2. Call utf8proc_map, passing the EXACT length of bytes instead of reading until '\0'
    utf8proc_uint8_t* normalized_ptr = nullptr;

    // IMPORTANT: Remove the UTF8PROC_NULLTERM flag, as we control the length in bytes ourselves
    utf8proc_option_t options = static_cast<utf8proc_option_t>(
        UTF8PROC_STABLE | UTF8PROC_COMPOSE
        );

    utf8proc_ssize_t result_len = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t*>(input_str),
        static_cast<utf8proc_ssize_t>(actual_input_len), // Explicitly pass the length in bytes
        &normalized_ptr,
        options
    );

    if (result_len < 0 || !normalized_ptr) {
        /*
            If we use nulling with an incorrect parse, when saving with an incorrect password,
            when decoding with this password, the UI will be empty.
            If we simply copy the original data, we will save 1:1 the binary garbage
            that we received during incorrect decoding BEFORE saving.
        */
        //std::memset(out_buf, 0, buffer_size);

        size_t copy_bytes = (input_size_bytes < buffer_size) ? input_size_bytes : buffer_size;
        std::memcpy(out_buf, input_str, copy_bytes);
        if (copy_bytes < buffer_size) {
            std::memset(out_buf + copy_bytes, 0, buffer_size - copy_bytes);
        }
        return false;
    }

    size_t nfc_len = static_cast<size_t>(result_len);

    // 3. Copy no more than the buffer can hold
    size_t bytes_to_copy = (nfc_len < buffer_size) ? nfc_len : buffer_size;

    if (bytes_to_copy > 0) {
        std::memcpy(out_buf, normalized_ptr, bytes_to_copy);
    }

    // 4. Zero the rest of the buffer
    if (bytes_to_copy < buffer_size) {
        std::memset(out_buf + bytes_to_copy, 0, buffer_size - bytes_to_copy);
    }

    // 5. Free the intermediate UTF8proc buffer
    std::memset(normalized_ptr, 0, nfc_len);
    std::free(normalized_ptr);

    return nfc_len <= buffer_size;
}

bool UTF8_to_NFC::to_clean_string(const uint8_t* input_bytes, size_t input_size_bytes,
    char* out_buf, size_t max_out_size)
{
    if (!out_buf || max_out_size == 0) {
        return false;
    }

    if (!input_bytes || input_size_bytes == 0) {
        out_buf[0] = '\0';
        return true;
    }

    // 1. Find the actual end of data before the null terminator or input_size_bytes
    size_t actual_length = 0;
    while (actual_length < input_size_bytes && input_bytes[actual_length] != 0x00) {
        actual_length++;
    }

    // 2. Calculate the amount to copy accounting for the null terminator space
    size_t max_copy_len = max_out_size - 1;
    size_t bytes_to_copy = (actual_length < max_copy_len) ? actual_length : max_copy_len;

    if (bytes_to_copy > 0) {
        std::memcpy(out_buf, input_bytes, bytes_to_copy);
    }

    // 3. Terminate the string
    out_buf[bytes_to_copy] = '\0';

    return actual_length <= max_copy_len;
}