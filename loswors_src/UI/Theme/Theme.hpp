#pragma once
#include <raylib.h>


class Theme {
public:
	Theme() = delete;

	/**
		@brief Get Curent Count of UI Theme
		@return Theme count
	*/
	static int GetThemeCount();

	/**
		@brief Set theme id & reload (const &)
		@param id:
			Settings::ui_theme_id -> (if normal)
			-1 -> (if random theme)
	*/
	static void SetTheme(int id);

	/**
		@brief Draw Bg in full screen (call after BeginDrawing())
		@param aa - full window aa
		@param id - if (normal id) -> draw bg by this id
	*/
	static void DrawBG(Rectangle aa, int id = -3, bool isDim = true);

	//       ___     _            
	//      / __|___| |___ _ _ ___
	//     | (__/ _ \ / _ \ '_(_-<
	//      \___\___/_\___/_| /__/
	// ---------------------------------------------------------
	struct C {
		C() = delete;

		static const Color& log_norm;
		static const Color& log_ok;
		static const Color& log_warning;
		static const Color& log_error;
		static const Color& logs_bg_def;
		static const Color& logs_bg_dark;
		static const Color& log_fullscreen_bg;
		static const Color& log_fullscreen_line;

		static const Color& setting_inactive_txt;
		static const Color& setting_lang_txt;
		static const Color& setting_fullhd_txt;
		static const Color& setting_fullhd_bg;
		static const Color& setting_div_line;

		static const Color& mst_fparcopy_bg_old;
		static const Color& mst_fparcopy_txt_old;
		static const Color& mst_fparcopy_bg_new;
		static const Color& mst_fparcopy_txt_new;
		static const Color& mst_fp_fpar_bg_warning;
		static const Color& mst_fp_fpar_txt_warning;
		static const Color& mst_fparcopy_txt;

		static const Color& timeblock_border;

		static const Color& dirview_file;

		static const Color& passgen_0_rt;
		static const Color& passgen_1_rt;

		static const Color& global_bg;
	};
	



	//      ___ ___         ___      _     ___ _        _     
	//     / __/ __|  ___  / __| ___| |_  / __| |_ _  _| |___ 
	//     \__ \__ \ |___| \__ \/ -_)  _| \__ \  _| || | / -_)
	//     |___/___/       |___/\___|\__| |___/\__|\_, |_\___|
	// ------------------------------------------- |__/ --------

	static void SS_db_log(Color txtColor, float txtH);
	static void SS_db_version(float txtH);
	static void SS_db_exit(float txtH);
	static void SS_db_log_scroll();
	static void SS_db_version_scroll();
	static void SS_db_version_scroll_btns();

	static void SS_setting_main_btn();
	static void SS_setting_checkbox(float txtH, float textPAD);
	static void SS_setting_input_timeout(float txtH);
	static void SS_setting_theme_choose();
	static void SS_setting_lang_choose_btn(float txtH);
	static void SS_setting_lang_choose_list(float txtH, float unitH);
	static void SS_setting_setdef_btn(float txtH);
	static void SS_setting_reloadsetwh_btn();

	static void SS_sst_checkbox(float txtH, float txtpad);
	static void SS_sst_12file(float txtH);
	static void SS_sst_newbtn();
	static void SS_sst_makenew(float txtH);
	static void SS_sst_passinput(float txtH);
	static void SS_sst_fdraw(float txtH);
	static void SS_sst_XX();

	static void SS_mst_file_info();
	static void SS_mst_search_checkbox(float txtH, float txtpad);
	static void SS_mst_save_btn(bool isActive);
	static void SS_mst_fparam_btn();
	static void SS_mst_puarr_scroll();
	static void SS_mst_pu_block();
	static void SS_mst_newbtn();
	static void SS_mst_search(float txtH);
	static void SS_mst_generator(float txtH);
	static void SS_mst_fp_pinverif();
	static void SS_mst_fp_fparcopy_btn_act();
	static void SS_mst_fp_fparcopy_btn_pas();
	static void SS_mst_fp_fparcopy_filedata(float txtH, float txtpad);
	static void SS_mst_fp_fparcopy_secretdata();
	static void SS_mst_fp_editpu(float txtH);
	
};