#pragma once
#include <cstdint>

namespace XOR_Shuffle {

	// update Data
	void for_GDATA_IN(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize);
	void for_GDATA_OUT(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize);

	// update Data
	void for_INSIDE_PU_IN(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize);
	void for_INSIDE_PU_OUT(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize);

}