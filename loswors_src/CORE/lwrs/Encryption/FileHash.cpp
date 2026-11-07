#include "FileHash.hpp"
#include "../../SecureWipe.hpp"

namespace FileHash {

	uint8_t* MAKE_FILE_HASH(const uint8_t* Data, size_t DSz, size_t reqSz) {
		if (reqSz == 0) return nullptr;

		uint8_t* hash = new uint8_t[reqSz];
		memset(hash, 0x47, reqSz);

		if (!Data || DSz == 0) return hash;

		for (size_t i = 0; i < DSz; ++i) {
			size_t hID = i % reqSz;

			hash[hID] ^= Data[i];
			hash[hID] *= 0x71;

			uint8_t rot = static_cast<uint8_t>((hash[hID] >> 3) | (hash[hID] << 5));
			hash[(hID + 1) % reqSz] ^= rot;
		}

		return hash;
	}

	bool IS_FILE_HASH_OK(const uint8_t* Data, size_t DSz, size_t HashSz) {
		if (HashSz >= DSz || !Data) return false;

		uint8_t* HASH = MAKE_FILE_HASH(Data, DSz - HashSz, HashSz);
		if (!HASH) return false;
		size_t offset2hash = DSz - HashSz;

		bool ret = true;
		for (size_t i = 0; i < HashSz; ++i) {
			if (HASH[i] != Data[offset2hash + i]) {
				ret = false;
				break;
			}
		}

		delete[] HASH; HASH = nullptr;
		return ret;
	}

}