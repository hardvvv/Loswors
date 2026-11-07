#pragma once
#include <raylib.h>

#include "cstdint"



namespace Const {

	//      ____            _      
	//     | __ )  __ _ ___(_) ___ 
	//     |  _ \ / _` / __| |/ __|
	//     | |_) | (_| \__ \ | (__ 
	//     |____/ \__,_|___/_|\___|
	// --------------------------------------------------------------------------------------------

	constexpr char		AUTHOR_NAME[9]				= "HARD_VVV";

	constexpr char		PROGRAM_NAME[8]				= "Loswors";
	constexpr char		LOSWORS_VERSION_TXT[14]		= "Version 1.0.0";
	constexpr uint16_t	LOSWORS_VERSION_INT[3]		= { 1, 0, 0 };

	constexpr char		PROGRAM_GITURL[35]			= "https://github.com/hardvvv/Loswors";

	constexpr char		LOSWORS_FILE_FORMAT[5]		= "lwrs";
	constexpr char		LOSWORS_TEMP_FILE_FORMAT[9] = "lwrsTEMP";

	//     __     ___                 _   ____        __             _ _   
	//     \ \   / (_)___ _   _  __ _| | |  _ \  ___ / _| __ _ _   _| | |_ 
	//      \ \ / /| / __| | | |/ _` | | | | | |/ _ \ |_ / _` | | | | | __|
	//       \ V / | \__ \ |_| | (_| | | | |_| |  __/  _| (_| | |_| | | |_ 
	//        \_/  |_|___/\__,_|\__,_|_| |____/ \___|_|  \__,_|\__,_|_|\__|
	// --------------------------------------------------------------------------------------------

	constexpr int	TARGET_FPS		= 60;

	constexpr int	WIDTH_DEF		= 840;
	constexpr int	HEIGHT_DEF		= 630;

	constexpr int	WIDTH_MIN		= 600;
	constexpr int	HEIGHT_MIN		= 450;

	constexpr int	WIDTH_MAX		= 1260;
	constexpr int	HEIGHT_MAX		= 945;

	constexpr int	WIN_HEADER_PX	= 37; // def 37

	constexpr float	TEXT_H_COEF_WH	= 0.032f;		// (TEXT_H_COEF_WH * min side)

	namespace Colors {
		constexpr Color BG_def			= { 41, 44, 51, 255 };

		constexpr Color TXT_def			= { 194, 200, 208, 255 };
		constexpr Color TXT_LOG_ok		= { 0, 255, 0 ,255 };
		constexpr Color TXT_LOG_warning	= { 253, 249, 0 ,255 };
		constexpr Color TXT_LOG_error	= { 255, 0, 0 ,255 };
	}

	constexpr float INTRO_DURATION_S	= 2.5f;


	//      ____                  _____ _ _      
	//     / ___|  __ ___   _____|  ___(_) | ___ 
	//     \___ \ / _` \ \ / / _ \ |_  | | |/ _ \
	//      ___) | (_| |\ V /  __/  _| | | |  __/
	//     |____/ \__,_| \_/ \___|_|   |_|_|\___|
	// --------------------------------------------------------------------------------------------

	// SaveFile --- \

	constexpr size_t	PIN_SALT_SIZE			= 32;		// |  byte
	constexpr size_t	PIN_HASH_SIZE			= 32;		// |______

	constexpr size_t	MASTER_PASS_SIZE		= 512;		// byte (in UI +1 for \0)
	constexpr size_t	MASTER_PASS_SALT_SIZE	= 512;		// byte
	constexpr size_t	MASTER_HASH_SIZE		= 512;		// byte

	constexpr size_t	FILE_CHECK_HASH_SIZE	= 64;		// byte

	constexpr size_t	FILE_WARNING_SIZE		= 146;
	constexpr uint8_t	FILE_WARNING_BYTES[]	= {
													"\n------------------------------------\n"	// 38 |
													"DO NOT EDIT THE FILE OUTSIDE LOSWORS\n"	// 37 |  total 146 + \0
													"YOU WILL LOSE ALL YOUR DATA ! ! !\n"		// 34 |  in RAW file write \0 IGNORE
													"------------------------------------\n"	// 37 |___________
												};

	// SaveData (PassUnit) --- /

	constexpr size_t	PU_HASH_SIZE		= 128;		// | hash size in (byte)

	constexpr size_t	PU_NAME_SALT_SIZE	= 32;		// |
	constexpr size_t	PU_PASS_SALT_SIZE	= 32;		// |  salt size in (byte)
	constexpr size_t	PU_DESC_SALT_SIZE	= 32;		// |_____________________

	constexpr size_t	PU_NAME_LEN			= 128;		// |
	constexpr size_t	PU_PASS_LEN			= 512;		// |  for store byte | for UI char* (for UI + 1)
	constexpr size_t	PU_DESC_LEN			= 512;		// |_________________________________________________

	//         _       _     _ _ _   
	//        / \   __| | __| (_) |_ 
	//       / _ \ / _` |/ _` | | __|
	//      / ___ \ (_| | (_| | | |_ 
	//     /_/   \_\__,_|\__,_|_|\__|
	// --------------------------------------------------------------------------------------------

	constexpr int64_t	TIME_AFTERSTART_TO_PROCESS_DATA_ns	= 1'000'000'000;	// 1s after start
	constexpr int64_t	TIME_FAST_MONOPING_ns				= 200'000'000;		// 0.2s need to elapse
	constexpr int64_t	TIME_NORMAL_MONOPING_ns				= 2'000'000'000;	// 2s to ping while normal

	constexpr int		MIN_INACTIVE_TIMEOUT_s = 10;	// 10 sec.
	constexpr int		MAX_INACTIVE_TIMEOUT_s = 600;	// 10 min.


	constexpr size_t	MAX_LOG_COUNT				= 2048;
	constexpr size_t	MAX_PATH_LEN				= 2048;		// for UI + 1 -> '\0'
	constexpr size_t	MAX_NEW_NAME_LEN			= 128;		// for UI + 1 -> '\0'	(for new loswors save file name)
	constexpr size_t	MAX_LANGUAGE_NAME			= 6;		// for UI + 1 -> '\0'	(for name like {"ru", "en", "BiH"})
	constexpr size_t	MAX_SEARCH_TEXT				= 128;		// for UI + 1 -> '\0'

}