#include "KDF.hpp"

#include "../../External/argon2/include/argon2.h"
#include "../../External/picosha2/picosha2.h"


// 1. Standard C++ libraries (REQUIRED FIRST!)
#include <random>
#include <chrono>
#include <thread>
#include <array>
#include <cstdint>
#include <cstddef>
#include <functional>
// 2. Cross-platform headers
#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wincrypt.h>
#include <intrin.h> // For __rdtsc()
#else
#include <sys/random.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(__x86_64__) || defined(__i386__)
#include <x86intrin.h>
#endif
#endif

// ============================================================================
// PRE-ALLOCATED ENTROPY (OUTSIDE NAMESPACE KDF)
// ============================================================================

namespace {

    // Gather system "noise" (CPU ticks, addresses, threads, high-precision timers)
    uint64_t GatherEnvironmentalEntropy() {
        uint64_t state = 0;

        // 1. CPU tick counter (RDTSC)
#if defined(_WIN32) || defined(_WIN64)
        state ^= __rdtsc();
#elif defined(__x86_64__) || defined(__i386__)
        state ^= __rdtsc();
#endif

        // 2. High-resolution system and thread timers
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        auto sys_now = std::chrono::system_clock::now().time_since_epoch().count();
        state ^= static_cast<uint64_t>(now);
        state ^= (static_cast<uint64_t>(sys_now) << 13);

        // 3. Data on system memory and threads
        state ^= reinterpret_cast<uintptr_t>(&state);
        state ^= std::hash<std::thread::id>{}(std::this_thread::get_id());

        return state;
}
// Initialize RNG at startup by immediately calling a lambda function
static std::mt19937_64 global_rng = []() {
    std::array<uint64_t, 8> seed_data{};

    // Source 1: std::random_device (RDRAND/RDSEED)
    std::random_device rd;
    for (size_t i = 0; i < 4; ++i) {
        seed_data[i] = (static_cast<uint64_t>(rd()) << 32) | rd();
    }

    // Source 2: System cryptographic API
#if defined(_WIN32) || defined(_WIN64)
        HCRYPTPROV hProvider = 0;
        if (CryptAcquireContextA(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            CryptGenRandom(hProvider, static_cast<DWORD>(sizeof(uint64_t) * 2), reinterpret_cast<BYTE*>(&seed_data[4]));
            CryptReleaseContext(hProvider, 0);
        }
#else
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd != -1) {
            ssize_t result = read(fd, &seed_data[4], sizeof(uint64_t) * 2);
            (void)result;
            close(fd);
        }
#endif

        // Source 3: Noise in Microarchitecture and Timers
        seed_data[6] = GatherEnvironmentalEntropy();
        seed_data[7] = GatherEnvironmentalEntropy() ^ (seed_data[0] << 7);

        std::seed_seq seq(seed_data.begin(), seed_data.end());
        return std::mt19937_64(seq);
        }();

    // Generate a new byte with a fresh RDTSC mix
    uint8_t GetEntropyByte() {
        uint64_t extra = GatherEnvironmentalEntropy();
        return static_cast<uint8_t>((global_rng() ^ extra) & 0xFF);
    }


    size_t Roll_back_ID(size_t id, size_t MAXid) {
        if (MAXid <= 1) return 0;
        size_t period = 2 * (MAXid - 1);
        size_t rem = id % period;
        return (rem >= MAXid) ? (period - rem) : rem;
    }

    inline void MixByte(uint8_t& hash, uint8_t byte) {
        hash ^= byte;
        hash = (hash << 3) | (hash >> 5);
    }
}


namespace KDF {

    uint8_t RANDOM() {
        return GetEntropyByte();
    }

    uint8_t* RandomSalt(size_t reqSize) {
        if (reqSize == 0) return nullptr;

        uint8_t* salt = new uint8_t[reqSize];
        for (size_t i = 0; i < reqSize; ++i) {
            salt[i] = GetEntropyByte();
        }
        return salt;
    }



    bool Argon2Interface(
        const uint8_t* pass, size_t pass_len,
        const uint8_t* salt, size_t salt_len,
        uint8_t* out_hash, size_t hash_len
    ) {

        uint32_t t_cost = 7;
        uint32_t m_cost = (1 << 18); // in KiB  |  2^18 = 262144
        uint32_t parallelism = 2;

        int r = argon2id_hash_raw(
            t_cost, m_cost, parallelism,
            pass, pass_len,     // uint8_t* -> void*
            salt, salt_len,
            out_hash, hash_len
        );

        if (r == ARGON2_OK) {
            return true;
        }
        return false;
    }

    void Sha256_PIN_Interface(
        const uint8_t* pass, size_t pass_len,
        const uint8_t* salt, size_t salt_len,
        uint8_t* out_hash, size_t hash_len
    ) {

        if (out_hash == nullptr || hash_len == 0) return;

        uint8_t hash32[32]; // constant 32 

        // (pass + sals/prev hash)
        size_t workdata_len = pass_len + std::max(salt_len, size_t(32));
        uint8_t* workdata = new uint8_t[workdata_len];

        // copy to buf
        memcpy(workdata, pass, pass_len);
        memcpy(workdata + pass_len, salt, salt_len);
        size_t input_len = pass_len + salt_len;

        picosha2::hash256(workdata, workdata + input_len, hash32, hash32 + 32);

        size_t filled = std::min(hash_len, size_t(32));
        memcpy(out_hash, hash32, filled);

        // generator if hash size req > 32 byte
        while (filled < hash_len) {
            
            memcpy(workdata, pass, pass_len);
            memcpy(workdata + pass_len, hash32, 32);
            input_len = pass_len + 32;

            picosha2::hash256(workdata, workdata + input_len, hash32, hash32 + 32);

            size_t chunk = std::min(hash_len - filled, size_t(32));
            memcpy(out_hash + filled, hash32, chunk);
            filled += chunk;
        }

        delete[] workdata;
    }


    // -------------------------- MY KDF --------------------------  // NO CRYPTO
    void Harpid(
        const uint8_t* key, size_t key_len,
        const uint8_t* salt, size_t salt_len,
        uint8_t* out_hash, size_t hash_len
    ) {
        if (key == nullptr || salt == nullptr
            || key_len < 2 || salt_len < 2 || hash_len < 2) return;
        memset(out_hash, 0, hash_len);

        int iter = 3;

        // make some determ noise in out_hash
        // but only 2^8 space
        for (size_t i = 0; i < hash_len * iter; i++) {
            uint8_t s = salt[Roll_back_ID(i, salt_len)];
            uint8_t p = key[Roll_back_ID(i, key_len)];

            size_t r_i = Roll_back_ID(i, hash_len);

            uint8_t stepxx = key[Roll_back_ID(p, key_len)];
            for (size_t isq = r_i; isq < r_i + stepxx; isq++) {
                out_hash[r_i] ^= key[Roll_back_ID(isq, key_len)];
                MixByte(out_hash[r_i], s);
            }

            out_hash[r_i] ^= salt[Roll_back_ID(s, salt_len)] ^ 0b10101010;
            out_hash[r_i] ^= key[Roll_back_ID(p, key_len)];

            if (r_i > 0) {
                out_hash[r_i - 1] += out_hash[r_i] ^ (p * (s < (1 << 7)) ? 17 : (p + 13)); // if < 128 
                out_hash[r_i] *= (out_hash[r_i - 1] | 1); // | 1 ) for no (* 0)
                out_hash[r_i] -= salt[Roll_back_ID(out_hash[r_i - 1] + i, salt_len)];
            }
        }
    }
    
}