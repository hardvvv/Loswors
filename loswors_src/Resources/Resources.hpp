#pragma once
#include <raylib.h>
#include <raygui.h>

class Resources {
private:

	static Image program_icon;

	// INTRO
	static Texture2D BG, Holder, Key, Z;

	// DECODE
	static Texture2D INPROG, INPROG_HOVER, INFILE, BIN_Shuffle, FileHash, Argon2, G_XOR_Shuffle, I_XOR_Shuffle;

	// THEME
	static Texture2D 
		tbg_1_coldfog,
		tbg_2_greenrailway,
		tbg_3_plantout,
		tbg_4_road;

	// PROGRAM
	static Texture2D 
		copy,			cros,			enter,			eye_close,
		eye_open,		file_check,		ok_gal,			reload,
		setting,		win_minim,		program_tx,		header_unit,
		search,			edit,			drop_file;

	static Font 
		FT_Main,
		FT_Main_BIGNAME,
		FT_MainMono;

public:
	Resources() = delete;

	// IMG
	static Image		GetIMG_program_icon();

	// INTRO
	static Texture2D	GetIntroTX_BG();
	static Texture2D	GetIntroTX_Holder();
	static Texture2D	GetIntroTX_Key();
	static Texture2D	GetIntroTX_Z();
	//DECODE
	static Texture2D	GetDcdTX_INPROG();
	static Texture2D	GetDcdTX_INPROG_HOVER();
	static Texture2D	GetDcdTX_INFILE();
	static Texture2D	GetDcdTX_BIN_Shuffle();
	static Texture2D	GetDcdTX_FileHash();
	static Texture2D	GetDcdTX_Argon2();
	static Texture2D	GetDcdTX_G_XOR_Shuffle();
	static Texture2D	GetDcdTX_I_XOR_Shuffle();
	// THEME
	static Texture2D	GetTbg_1_coldfog();
	static Texture2D	GetTbg_2_greenrailway();
	static Texture2D	GetTbg_3_plantout();
	static Texture2D	GetTbg_4_road();
	// PROGRAM
	static Texture2D	GetTX_copy();
	static Texture2D	GetTX_cros();
	static Texture2D	GetTX_enter();
	static Texture2D	GetTX_eye_close();
	static Texture2D	GetTX_eye_open();
	static Texture2D	GetTX_file_check();
	static Texture2D	GetTX_ok_gal();
	static Texture2D	GetTX_reload();
	static Texture2D	GetTX_setting();
	static Texture2D	GetTX_win_minim();
	static Texture2D	GetTX_program_icon();
	static Texture2D	GetTX_header_unit();
	static Texture2D	GetTX_search();
	static Texture2D	GetTX_edit();
	static Texture2D	GetTX_drop_file();
	// FONT
	static Font			GetFT_main_font();
	static Font			GetFT_main_BIGNAME_font();
	static Font			GetFT_main_font_mono();

	
	static void ReloadFont(); // use after Localization _INIT_ reload font
	static void LoadAll();
	static void ClearData();
};