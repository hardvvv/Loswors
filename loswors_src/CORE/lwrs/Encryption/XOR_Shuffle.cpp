#include "XOR_Shuffle.hpp"
#include "../../SecureWipe.hpp"
#include <string>

namespace {
	// [0, MAXs - 1]
	inline size_t normID(size_t Gi, int8_t IDpp, size_t MAXs) {
		int64_t rID = static_cast<int64_t>(Gi) + IDpp;

		if (rID < 0) {
			rID += MAXs;
		}
		else if (rID >= static_cast<int64_t>(MAXs)) {
			rID -= MAXs;
		}

		return static_cast<size_t>(rID);
	};

	void DetermRandExpand(uint8_t* XORmask, size_t DATAsize, const uint8_t* KEY, size_t KEYsize) {
		memcpy(XORmask, KEY, KEYsize);

		size_t cpy_id = 0;
		size_t pst_id = KEYsize;

		while (pst_id < DATAsize) {
			// get val from hash for expand (determ. random expand)
			// The index selection window is shifted to use the following indices, the mask size must be >= 256 !
			size_t read_id = (cpy_id + XORmask[cpy_id]) % pst_id;

			XORmask[pst_id] = XORmask[read_id];
			pst_id++;
			cpy_id++;
		}
	}

	size_t Roll_back_ID(size_t id, size_t MAXid) {
		if (MAXid <= 1) return 0;
		size_t period = 2 * (MAXid - 1);
		size_t rem = id % period;
		return (rem >= MAXid) ? (period - rem) : rem;
	}
}

namespace XOR_Shuffle {

	void for_GDATA_IN(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize) {
		if (DATAsize < 256 || DATAsize < KEYsize || !DATA || !KEY || KEYsize == 0) return;

		uint8_t* XORmask = new uint8_t[DATAsize]; memset(XORmask, 0, DATAsize);

		DetermRandExpand(XORmask, DATAsize, KEY, KEYsize);

		// STEP 2
		// -------------------
		for (size_t i = 0; i < DATAsize; i++) {
			DATA[i] ^= XORmask[i];
		}

		// STEP 1
		// -------------------
		int start_i = (DATAsize % 2 == 0) ? (DATAsize - 1) : (DATAsize - 2);
		for (size_t q = 0; q < 7; ++q) {
			for (int i = start_i; i >= 1; i -= 2) {
				size_t u0 = normID(i, static_cast<int8_t>(XORmask[i - 1] - 127), DATAsize);
				size_t u1 = normID(i, static_cast<int8_t>(XORmask[i] - 127), DATAsize);

				if (u0 != u1) {
					std::swap(DATA[u0], DATA[u1]);
				}
			}
		}

		SecureWipe(XORmask, DATAsize);
		delete[] XORmask; XORmask = nullptr;
	}
	void for_GDATA_OUT(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize) {
		if (DATAsize < 256 || DATAsize < KEYsize || !DATA || !KEY || KEYsize == 0) return;

		uint8_t* XORmask = new uint8_t[DATAsize]; memset(XORmask, 0, DATAsize);

		DetermRandExpand(XORmask, DATAsize, KEY, KEYsize);

		// STEP 1
		// -------------------
		for (size_t q = 0; q < 7; ++q) {
			for (size_t i = 1; i < DATAsize; i += 2) {
				size_t u0 = normID(i, static_cast<int8_t>(XORmask[i - 1] - 127), DATAsize);
				size_t u1 = normID(i, static_cast<int8_t>(XORmask[i] - 127), DATAsize);
				// xor swap
				if (u0 != u1) {
					std::swap(DATA[u0], DATA[u1]);
				}
			}
		}

		// STEP 2
		// -------------------
		for (size_t i = 0; i < DATAsize; i++) {
			DATA[i] ^= XORmask[i];
		}

		SecureWipe(XORmask, DATAsize);
		delete[] XORmask; XORmask = nullptr;
	}

	void for_INSIDE_PU_IN(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize) {
		if (!KEY || !DATA || DATAsize <= 1 || KEYsize == 0) return;

		// STEP 2
		// -------------------
		for (size_t i = 0; i < DATAsize; ++i) {
			DATA[i] ^= KEY[Roll_back_ID(i, KEYsize)];
		}

		// STEP 1
		// -------------------
		size_t limit = DATAsize - (DATAsize % 2);
		for (size_t i = 0; i < limit; i += 2) {
			if (KEY[Roll_back_ID(i, KEYsize)] % 2 == 0) {
				std::swap(DATA[i], DATA[i + 1]);
			}
			else {
				DATA[i+1] ^= DATA[i]; // B ^= A
			}
		}
	}
	void for_INSIDE_PU_OUT(uint8_t* DATA, size_t DATAsize, const uint8_t* KEY, size_t KEYsize) {
		if (!KEY || !DATA || DATAsize <= 1 || KEYsize == 0) return;

		// STEP 1
		// -------------------
		size_t limit = DATAsize - (DATAsize % 2);
		for (size_t i = 0; i < limit; i += 2) {
			if (KEY[Roll_back_ID(i, KEYsize)] % 2 == 0) {
				std::swap(DATA[i], DATA[i + 1]);
			}
			else {
				DATA[i + 1] ^= DATA[i]; // B ^= A
			}
		}

		// STEP 2
		// -------------------
		for (size_t i = 0; i < DATAsize; ++i) {
			DATA[i] ^= KEY[Roll_back_ID(i, KEYsize)];
		}
	}

}