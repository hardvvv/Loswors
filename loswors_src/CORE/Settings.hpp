#pragma once
#include "Const.hpp"


class Settings {
public:
	Settings() = delete;

	// status
	static bool is_2file;
	static bool is_forget_after_exit;
	static bool is_no_file_exit;
	static bool is_show_decode_anim;
	static bool is_random_theme;

	// file path
	static char path_f0[Const::MAX_PATH_LEN + 1];
	static char path_f1[Const::MAX_PATH_LEN + 1];

	// UI
	static int Width;
	static int Height;
	static int ui_theme_id;

	// localization
	static char language[Const::MAX_LANGUAGE_NAME + 1];

	// Addit
	static int inactive_timeout_sec;



	static void Load();

	// raw write !! secure write need befor!!
	static void Save();

	static void ClearFPath0();
	static void ClearFPath1();
	static void SetDef();

};