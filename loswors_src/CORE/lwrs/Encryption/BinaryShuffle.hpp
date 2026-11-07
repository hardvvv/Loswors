#pragma once
#include <cmath>

namespace BinaryShuffle {

	/*
		10101010
		10101010
		10101010
		10101010
		10101010
		10101010

		to

		D0
		11111111
		11111111
		11111111

		D1
		00000000
		00000000
		00000000

		split data in binary lvl to 2 arr bit by bit
		in end set 00000000 if rawData % 16 else 11111111

		return size for D0 & D1  & update arr D0 D1
	*/
	size_t Shuffle(const uint8_t* rawData, size_t datalen, uint8_t*& D0, uint8_t*& D1);

	/*
		2 data arr into 1 arr bit to bit
		D0(01100101)
		D1(11101110)
		rawDATA(
			01111100
			01110110
		)

		if end = 11111111
		size -8 -4 (in D0 D1)
		if end = 00000000
		size -8    (in D0 D1)

		return pointer to new arr & update size
	*/
	uint8_t* UN_Shuffle(const uint8_t* D0, const uint8_t* D1, size_t Ds_size, size_t& NewSize);

};