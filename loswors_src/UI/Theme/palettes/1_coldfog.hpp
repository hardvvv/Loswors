#pragma once
#include "0_genesis.hpp"

/*
* 
#define TEXT_DEF { 194, 200, 208, 255 }
#define TEXT_PRES_DEF { 222, 222, 222, 255 }
#define LINE_DEF { 102, 115, 132, 255 }
#define LINE_PRES_DEF { 24, 27, 30, 255 }
#define BG_DEF { 24, 27, 30, 255 }
#define BG_PRES_DEF { 172, 60, 60, 255 }
#define LOG_DB_DEF { 41, 44, 51, 255 }

struct palette {

	// log ----------------------------
	Color log_norm = TEXT_DEF;
	Color log_ok = GREEN;
	Color log_warning = YELLOW;
	Color log_error = RED;

	Color logs_bg_def = LOG_DB_DEF;
	Color logs_bg_dark = { 26, 29, 36, 255 };

	Color log_scroll_bg = LOG_DB_DEF;

	// def var ------------------------
	Color def_text_norm = TEXT_DEF;
	Color def_text_actv = { 2, 2, 2, 255 };
	Color def_text_pres = TEXT_PRES_DEF;

	Color def_bg_norm = BG_DEF;
	Color def_bg_actv = { 167, 175, 176, 255 };
	Color def_bg_pres = BG_PRES_DEF;

	Color def_line_norm = LINE_DEF;
	Color def_line_actv = { 211, 219, 223, 255 };
	Color def_line_pres = BG_DEF;

	// settings -----------------------
	Color setting_checkbox_text_norm = TEXT_DEF;
	Color setting_checkbox_text_pres = TEXT_PRES_DEF;

	Color setting_checkbox_line_norm = LINE_DEF;
	Color setting_checkbox_line_actv = BG_PRES_DEF;
	Color setting_checkbox_line_pres = BG_DEF;

	Color setting_inactive_text_norm = { 104, 168, 208, 255 };

	Color setting_fullhd_text = { 128, 128, 128, 255 };
	Color setting_fullhd_bg = BG_DEF;
	Color setting_div_line = { 0, 0, 0, 100 };

	// main state
	Color mst_save_btn_bg_active	= { 0, 76, 0, 255 };
	Color mst_fp_fparcopy_btn_bg_act = { 0, 113, 40, 255 };
	Color mst_fp_fparcopy_bg_old	= { 255, 0, 0, 40 };
	Color mst_fp_fparcopy_txt_old	= { 255, 0, 0, 200 };
	Color mst_fp_fparcopy_bg_new	= { 0, 255, 0, 40 };
	Color mst_fp_fparcopy_txt_new	= { 0, 255, 0, 128 };
	Color mst_fp_fpar_bg_warning	= { 255, 255, 0, 40 };
	Color mst_fp_fpar_txt_warning	= { 255, 255, 0, 200 };
	Color mst_fp_fparcopy_txt		= TEXT_DEF;

	// global -------------------------
	Color bg = { 41, 44, 51, 255 };
	Color db_exit_text = RED;
	Color db_version_text = BLUE;

};

inline constexpr palette _0_genesis{};

#undef TEXT_DEF
#undef TEXT_PRES_DEF
#undef LINE_DEF
#undef LINE_PRES_DEF
#undef BG_DEF
#undef BG_PRES_DEF
#undef LOG_DB_DEF

*/


#define LINE_DEF { 102, 115, 132, 255 }
#define LINE_PRES_DEF { 24, 27, 30, 255 }
#define BG_PRES_DEF { 172, 60, 60, 255 }


inline constexpr palette p1_coldfog{

	.setting_checkbox_line_norm = LINE_PRES_DEF,
	.setting_checkbox_line_actv = BG_PRES_DEF,
	.setting_checkbox_line_pres = LINE_DEF,

	.setting_theme_checkbox_line_norm = LINE_DEF,
	.setting_theme_checkbox_line_actv = BG_PRES_DEF,
	.setting_theme_checkbox_line_pres = LINE_PRES_DEF,

	.timeblock_border = LINE_PRES_DEF,

	.dirview_file = { 24, 27, 30, 255 },

	.passgen_0_rt = { 0, 0, 0, 90 },
	.passgen_1_rt = { 0, 0, 0, 40 }

};

#undef LINE_DEF
#undef LINE_PRES_DEF
#undef BG_PRES_DEF