#pragma once
#include <cstdint>

namespace FileHash {

	// need full data (whithout hash...)
	uint8_t* MAKE_FILE_HASH(const uint8_t* Data, size_t DSz, size_t reqSz);

	// need full data
	bool IS_FILE_HASH_OK(const uint8_t* Data, size_t DSz, size_t HashSz);
	
}