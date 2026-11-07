/*

	Данный набор функций реализует взаемодействие между версиями файла,
	версия файла сохранения это 2й номер в версии, z.x.y где x это номер версии файла

	При обновлении кодировки файла нужно перенести функции (UNV_ENCODE_TO_PU, UNV_GET_DATA_CAST) прошлой версии в другое место,
	Обновленные функции (UNV_ENCODE_TO_PU, UNV_GET_DATA_CAST) видя файл меньшей версии чем текуший билд будут
	вызывать функции (UNV_ENCODE_TO_PU, UNV_GET_DATA_CAST) нужной версии,
	эти функции будут заполнять LwrsFileUnit который являеться своеобразным интерфейсом между версиями,
	это точка которая создает своеобразный API мост между версиями, где высшая версия может работать с низшими

	--------------------

	The following set of functions implements interaction between file versions.
	The save file version is the second number in the version z.x.y, where x is the file version number.

	When updating the file encoding, the functions (UNV_ENCODE_TO_PU, UNV_GET_DATA_CAST) from the previous version need to be moved to another location.
	The updated functions (UNV_ENCODE_TO_PU, UNV_GET_DATA_CAST), when they detect a file with a version lower than the current build,
	will call the functions (UNV_ENCODE_TO_PU, UNV_GET_DATA_CAST) of the required version. These functions will populate LwrsFileUnit,
	which serves as a kind of interface between versions.
	This is the point that creates a kind of API bridge between versions, where a higher version can work with lower versions.

*/

#pragma once 
#include <cstdint>

#include "LwrsFileUnit.hpp"

namespace UNVERSION {

	bool CheckPIN(
		const uint8_t(&PIN)[4],
		const uint8_t* PINsalt, size_t PINsalt_size,
		const uint8_t* PINhash, size_t PINhash_size
	);

	LwrsStatusOperation UNV_ENCODE_TO_PU(
		const uint8_t* data, size_t data_size,
		const uint8_t* MPASS, size_t MPASS_SIZE,
		uint16_t& Vmain, uint16_t& Vfile, uint16_t& Vadd,
		float& Entropy08,
		int64_t& Ns_Argon2,
		int64_t& Ns_XOR_MASK_Suffle,
		int64_t& Ns_InsidePU_XOR_MASK_Suffle,
		uint8_t*& PINsalt, size_t& PINsalt_size,
		uint8_t*& PINhash, size_t& PINhash_size
	);

	uint8_t* UNV_GET_DATA_CAST(
		size_t& new_data_size, const uint8_t(&PIN)[4],
		const uint8_t* MPASS, size_t MPASS_SIZE
	);
};