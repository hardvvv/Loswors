#pragma once
#include <cstdint>

#include "../Const.hpp"
#include "../Timer/timer.hpp"

enum class LwrsStatusOperation {
	OK,
	DownVersion,
	ErrorUpVersion,
	ErrorSize,
	ErrorFormat,		// not lwrs
	ErrorARGON2
};

struct PU
{
	int ConstID = 0;	// in file

	float sortVal = 0.0f;
	char name[Const::PU_NAME_LEN + 1] = {};
	char pass[Const::PU_PASS_LEN + 1] = {};
	char desc[Const::PU_DESC_LEN + 1] = {};

	void CopyParam(const char* NEW_name, const char* NEW_pass, const char* NEW_desc);
	void Clear();
};


class LwrsFileUnit {
private:

	static PU* PU_arr;
	static size_t Size_PU_arr;

	/*
		PIN LOGIC:
		sha256(sha256(PIN_SALT, MASTER_HASH), PIN) where PIN = PIN input from user(while save file) / (while GetDataForSave)
		result of first operation - sha256(PIN_SALT, MASTER_HASH) occurs when opening a file and saved in PINSALT
	*/
	static uint8_t* PINSALT;
	static size_t PINSALT_size;
	static uint8_t* PINHASH;
	static size_t PINHASH_size;

	static char version[18];	// 11111.22222.33333  + \0     {2^16 -> 65536 (5 simbols)}
	static size_t filedata_BYTE;
	static timer t_from_start;
	static float ShannonEntropy08;
	static int64_t ns_agron2;
	static int64_t ns_XOR_MASK_Suffle;
	static int64_t ns_InsidePU_XOR_MASK_Suffle;

public:

	LwrsFileUnit() = delete;

	static char*	GetVersionTEXT();
	static size_t	GetFileSizeBYTE();
	static int64_t	GetNs_FromOpen();
	static float	Get_Entropy08();
	static int64_t	GetNs_Argon2();
	static int64_t	GetNs_XOR_MASK_Suffle();
	static int64_t	GetNs_InsidePU_XOR_MASK_Suffle();


	static size_t GetPUCount();

	static const PU& GetPUByfloatID(int id);
	// for UI main user add logic
	static void AddPU(const char* NEW_name, const char* NEW_pass, const char* NEW_desc);
	// for inside use only (use for fill arr in normal sequences)
	static void AddPU_Back(const char* NEW_name, const char* NEW_pass, const char* NEW_desc);
	static void ChangePU(int const_id, const char* NEW_name, const char* NEW_pass, const char* NEW_desc);
	static void DeletePU(int const_id);

	static bool MovePU(int const_id, bool isUPmove);
	static bool MovePUtoID(int const_id, int const_id_target);

	static void NormalizePUarr();
	static void QQ_Search(const char* QQ_txt, bool is_nameQ, bool is_descQ);

	static bool isPINreal(const uint8_t(&PIN)[4]);

	static LwrsStatusOperation FillSelf(
		const uint8_t* data, size_t data_size,
		const uint8_t* MPASS, size_t MPASS_SIZE
	);
	static LwrsStatusOperation FillSelf_DOUBLE(
		const uint8_t* data0, size_t data0_size,
		const uint8_t* data1, size_t data1_size,
		const uint8_t* MPASS, size_t MPASS_SIZE
	);

	static uint8_t* GetDataForSave(size_t& new_data_size, const uint8_t(&PIN)[4], const uint8_t* MPASS, size_t MPASS_SIZE);

	static void DATA_DOUBLE(
		const uint8_t* DATA, size_t DATAsize,
		uint8_t*& data0, size_t& new_d0_size,
		uint8_t*& data1, size_t& new_d1_size
	);

	// befor all logic
	static void ClearSelf();

};