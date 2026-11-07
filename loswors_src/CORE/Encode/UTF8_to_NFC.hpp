#pragma once
#include <cstdint>

class UTF8_to_NFC {
public:
    UTF8_to_NFC() = delete;

    /**
     * API 1: Accepts a char* buffer of a specific size in bytes.
     *
     * - Handles even if input_str does NOT contain a '\0'.
     * - Stops reading at the first '\0' or when input_size_bytes is reached.
     * - Writes meaningful data to out_buf and fills the remaining out_buf with zeros.
     *
     * @param input_str        Input buffer char*
     * @param input_size_bytes EXACT size of the input buffer char* in bytes
     * @param out_buf          Target array uint8_t
     * @param buffer_size      Size of the target array out_buf in bytes
     * @return true if all data is successfully stored, false if truncated or an error occurs
     */
    static bool to_nfc_bytes(const char* input_str, size_t input_size_bytes,
        uint8_t* out_buf, size_t buffer_size);

    /**
     * API 2: Converts a uint8_t array to a char* buffer with explicit sizes at both ends.
     *
     * - Stops reading at the first '\0' in input_bytes or upon reaching input_size_bytes.
     * - Ensures that '\0' is placed at the end of out_buf.
     *
     * @param input_bytes      Input array of uint8_t
     * @param input_size_bytes Size of the input array in bytes
     * @param out_buf          Target buffer of char*
     * @param max_out_size     Maximum size of out_buf in bytes (INCLUDING the terminating '\0')
     * @return true if everything fits; false if truncation occurred
     */
    static bool to_clean_string(const uint8_t* input_bytes, size_t input_size_bytes,
        char* out_buf, size_t max_out_size);
};