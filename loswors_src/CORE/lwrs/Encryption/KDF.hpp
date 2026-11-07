#pragma once
#include <cstdint>

namespace KDF {

    uint8_t RANDOM();
	uint8_t* RandomSalt(size_t reqSize);

    bool Argon2Interface(
        const uint8_t* pass, size_t pass_len,
        const uint8_t* salt, size_t salt_len,
        uint8_t* out_hash, size_t hash_len
    );

    void Sha256_PIN_Interface(
        const uint8_t* pass, size_t pass_len,
        const uint8_t* salt, size_t salt_len,
        uint8_t* out_hash, size_t hash_len
    );

    void Harpid(
        const uint8_t* key, size_t key_len,
        const uint8_t* salt, size_t salt_len,
        uint8_t* out_hash, size_t hash_len
    );
}