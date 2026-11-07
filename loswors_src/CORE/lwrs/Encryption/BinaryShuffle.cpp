#include "BinaryShuffle.hpp"

namespace BinaryShuffle {

    size_t Shuffle(const uint8_t* rawData, size_t datalen, uint8_t*& D0, uint8_t*& D1) {
        if (!rawData || datalen == 0) {
            D0 = nullptr;
            D1 = nullptr;
            return 0;
        }

        size_t all_bits = datalen * 8;

        size_t half_bytes_count = (datalen + 1) / 2;
        size_t fs_size = half_bytes_count + 1; // +1 flag

        D0 = new uint8_t[fs_size]();
        D1 = new uint8_t[fs_size]();

        for (size_t i = 0; i < all_bits; i++) {
            bool bit = (rawData[i / 8] >> (7 - (i % 8))) & 1;

            size_t target_bit_idx = i / 2;
            size_t byte_pos = target_bit_idx / 8;
            size_t bit_pos = 7 - (target_bit_idx % 8);

            if (i % 2 == 0) {
                if (bit) D0[byte_pos] |= (1 << bit_pos);
            }
            else {
                if (bit) D1[byte_pos] |= (1 << bit_pos);
            }
        }

        // 0xFF 4 real bit et end  | 0x00 read all 8 bit et end
        uint8_t flag = (datalen % 2 != 0) ? 0xFF : 0x00;
        D0[fs_size - 1] = flag;
        D1[fs_size - 1] = flag;

        return fs_size;
    }

    uint8_t* UN_Shuffle(const uint8_t* D0, const uint8_t* D1, size_t Ds_size, size_t& NewSize) {
        if (!D0 || !D1 || Ds_size < 2) {
            NewSize = 0;
            return nullptr;
        }

        size_t data_bytes_count = Ds_size - 1; // half
        size_t total_bits = 0;

        if (D0[Ds_size - 1] == 0xFF) {
            total_bits = ((data_bytes_count - 1) * 8 + 4) * 2;
        }
        else {
            total_bits = data_bytes_count * 8 * 2;
        }

        NewSize = total_bits / 8;
        uint8_t* rawData = new uint8_t[NewSize]();

        for (size_t i = 0; i < total_bits; i++) {
            bool bit = false;
            size_t source_bit_idx = i / 2;
            size_t byte_pos = source_bit_idx / 8;
            size_t bit_pos = 7 - (source_bit_idx % 8);

            if (i % 2 == 0) {
                bit = (D0[byte_pos] >> bit_pos) & 1;
            }
            else {
                bit = (D1[byte_pos] >> bit_pos) & 1;
            }

            if (bit) {
                size_t out_byte_pos = i / 8;
                size_t out_bit_pos = 7 - (i % 8);
                rawData[out_byte_pos] |= (1 << out_bit_pos);
            }
        }

        return rawData;
    }

};















