#include "Start_ST.hpp"
#include <raygui.h>

#include <algorithm>

#include "../../CORE/Settings.hpp"
#include "../../CORE/FS/FileSys.hpp"
#include "../../CORE/FS/FileViewer.hpp"
#include "../../Localization/loc.hpp"

#include "../UI.hpp"
#include "../UI_Obj.hpp"
#include "../NotifService.hpp"
#include "../Elements/Settings_PN.hpp"
#include "../../Resources/Resources.hpp"
#include "../Theme/Theme.hpp"

#include <cmath>


#define RT_LINE_TEST 0

#if (RT_LINE_TEST == 1)
static inline void C(Rectangle rt) {
	DrawRectangleLinesEx(rt, 1.0f, RED);
}
#else
static inline void C(Rectangle) {}
#endif


//      _   _ ___                  _             
//     | | | |_ _|  _ __  __ _ _ _| |___  _ _ __ 
//     | |_| || |  | '  \/ _` | '_| / / || | '_ \
//      \___/|___| |_|_|_\__,_|_| |_\_\\_,_| .__/
// --------------------------------------- |_| -----------

constexpr float

	// def path || delete path after exit
	chekbox_xy1 = 0.03f,

	// main file draw zone
	m_cont_zone_x0 = 0.1f,		m_cont_zone_y0 = 0.17f,
	m_cont_zone_x1 = 0.8f,		m_cont_zone_y1 = 0.50f,

	// check slider (1 || 2) file
	f_count_x1 = 0.15f,			f_count_y1 = 0.07f,
	f_count_Hpad = 0.03f,

	// file choose unit & path
	path_pad_local = 0.02f,
	path_y1 = 0.16f,
	f_unit_pad_WH = 0.1f,
	SwapBTN_y0 = 0.3f,					// by inside zone for 2 file 
	SwapBTN_xy1_wh = 0.13f,				//

	check_x0 = 0.56f,					// param for check in choose file (cros ok_gal)
	check_y0 = 0.46f,					//
	check_xy1_WH_f_texture_size = 0.38f,

	// new make
	new_btn_txt_multip = 1.3f,			// normal_text_size * new_btn_txt_multip = large_tex
	new_btn_text_pad = 0.03f,
	new_btn_padding_text_WH = 0.01f,
	new_btn_padding_WH = 0.02f,			// i use this val from settings btn (so.. dont need)

	// make new visual  % all
	new_txt_multip = 1.6f,
	makenew_element_padding = 0.03f,
	makenew_name_x1 = 0.25f,
	makenew_name_y1 = 0.07f,
	makenew_path_x1 = 0.95f,			// for file zone

	// MASTER PASS param
	mpass_txt_multiply = 2.2f,			// normal_text_size * mpass_txt_multiply = large_tex
	mpass_txtbar_y0 = 0.77f,
	mpass_txtbar_x1 = 0.6f,				// text bar, center by x
	mpass_txtbar_y1 = 0.08f,

	mpass_pad_lr_btns = 0.012f,			// padding left right after txt bar (width)
	mpass_HdSw_y1 = 0.045f,

	enter_pass_y0 = 0.72f,				// beginning of the section with password entry
	y_between_enterconfirm = 0.05f,		// distance between the end of the input and the start of the confirmation


	t = 0; // delete t  (its only for norm readinfg)


namespace {

//      ___                        
//     | _ \__ _ _ _ __ _ _ __  ___
//     |  _/ _` | '_/ _` | '  \(_-<
//     |_| \__,_|_| \__,_|_|_|_/__/
// ----------------------------------------------------------------------------------
	bool is_MakeNewFile = false; // update in draw (get val from core)

	bool is_active_f0 = false, is_active_f1 = false;
	bool is_active_MPASS = true; bool is_hide_MPASS = true;

	bool is_hide_PASS_ENTER = true, is_hide_PASS_CONFIRM = true;
	bool is_active_enterpass = false, is_active_confirmpass = false;

	bool is_name_select = false;
	bool is_pin_hide = true;
	int active_PIN_ID = -1;
	bool is_new_active_f0 = false, is_new_active_f1 = false;


	char DOTlwrsTXT[16] = ".lwrs";


	// befor exit to main
	inline void ResetAllStatus() {
		is_active_f0 = false;		is_active_f1 = false;
		is_active_MPASS = true;		is_hide_MPASS = true;

		is_hide_PASS_ENTER = 0;			is_hide_PASS_CONFIRM = 0;
		is_active_enterpass = false;	is_active_confirmpass = false;

		is_name_select = false;
		is_pin_hide = true;
		active_PIN_ID = -1;
		is_new_active_f0 = false;	is_new_active_f1 = false;
	}

//      ___ _ _       ____             
//     | __(_) |___  |_  /___ _ _  ___ 
//     | _|| | / -_)  / // _ \ ' \/ -_)
//     |_| |_|_\___| /___\___/_||_\___|
// ----------------------------------------------------------------------------------
	inline void DrawOnesFileCkUnit(
		const Rectangle& f_unit_zone, const float pad_wh_icon, const float spase_path_h, const float PX_path_pad,
		const float min_param_ficon, char* path, bool& is_path_active, Color ok, Color def, int id_file) 
	{
		GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(Const::Colors::TXT_def));

		Rectangle n = {
			f_unit_zone.x + pad_wh_icon + (f_unit_zone.width - min_param_ficon) * 0.5f,
			f_unit_zone.y + pad_wh_icon + ((f_unit_zone.height - spase_path_h) - min_param_ficon) * 0.5f,
			min_param_ficon - pad_wh_icon * 2,
			min_param_ficon - pad_wh_icon * 2
		};
		bool is_ok_dir = FileSys::Ping_FileExists(path);
		Texture2D f_tx = is_ok_dir ? Resources::GetTX_ok_gal() : Resources::GetTX_cros();
		Color f_check_color = is_ok_dir ? GREEN : RED;
		Color f_color = is_ok_dir ? ok : def;
		
		if (UI_Obj::Hide_BTN_Texture(n, Resources::GetTX_file_check(), f_color)) {
			FileViewer::OpenFile(path, Const::MAX_PATH_LEN + 1, loc::t.pathview.selectfile);
		}
		UI_Obj::DrawTexture_Color(
			{
				n.x + (check_x0 * n.width),
				n.y + (check_y0 * n.height),
				check_xy1_WH_f_texture_size * n.width,
				check_xy1_WH_f_texture_size * n.height
			}, f_tx, f_check_color
		);
		
		Rectangle f_unit_path = {
			f_unit_zone.x + PX_path_pad,
			f_unit_zone.y + f_unit_zone.height - spase_path_h + PX_path_pad,
			f_unit_zone.width - PX_path_pad * 2,
			spase_path_h - PX_path_pad * 2
		};
		Theme::SS_sst_XX();
		if (UI_Obj::BTN_X16(f_unit_path.x, f_unit_path.y, 3)) {
			if (id_file == 0) {
				Settings::ClearFPath0();
			}
			else if (id_file == 1) {
				Settings::ClearFPath1();
			}
		}
		//GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
		GuiTextBox(
			f_unit_path,
			path, Const::MAX_PATH_LEN + 1, is_path_active
		) ? is_path_active = !is_path_active : is_path_active = is_path_active;
	}
	// drav on full area (bild in min max)!!
	inline void DrawFile_select(Rectangle aa, Rectangle m_zone) {

		Theme::SS_sst_fdraw(std::round(std::min(m_zone.width, m_zone.height) * 0.075f));

		Color def_color_icon = GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_NORMAL));
		Color ok_file_color = {
			def_color_icon.r,
			185,
			def_color_icon.b,
			def_color_icon.a
		};
	
		// zone for one def element (no pad)
		Rectangle f_unit_zone = {
			m_zone.x,
			m_zone.y,
			m_zone.width * 0.5f,
			m_zone.height
		};
		Rectangle f_unit_zone_center = {
			m_zone.x + m_zone.width * 0.5f - (f_unit_zone.width * 0.5f),
			f_unit_zone.y,
			f_unit_zone.width,
			f_unit_zone.height
		};
		Rectangle f_unit_zone_lft = f_unit_zone;
		Rectangle f_unit_zone_rgt = {
			m_zone.x + f_unit_zone.width,
			f_unit_zone.y,
			f_unit_zone.width,
			f_unit_zone.height
		};
		float sizeSwapBTN = std::round(std::min(m_zone.width, m_zone.height) * SwapBTN_xy1_wh);
		Rectangle SwapPathBTNz = {
			m_zone.x + (m_zone.width - sizeSwapBTN) * 0.5f,
			m_zone.y + m_zone.height * SwapBTN_y0,
			sizeSwapBTN, sizeSwapBTN };
	
		float minWH_size_FUZ = std::min(f_unit_zone.width, f_unit_zone.height);
		float PX_path_pad = minWH_size_FUZ * path_pad_local;
		float spase_path_h = f_unit_zone.height * path_y1;
	
		// WH param for file icon
		float min_param_ficon = std::min(f_unit_zone.height - spase_path_h, f_unit_zone.width);
		float pad_wh_icon = f_unit_pad_WH * min_param_ficon;
	
		if (!Settings::is_2file) {
			// 111
			DrawOnesFileCkUnit(
				f_unit_zone_center, pad_wh_icon, spase_path_h, PX_path_pad,
				min_param_ficon, Settings::path_f0, is_active_f0, ok_file_color, def_color_icon, 0);
	
			C(f_unit_zone_center);
		}
		else {
			// 222
			DrawOnesFileCkUnit(
				f_unit_zone_lft, pad_wh_icon, spase_path_h, PX_path_pad,
				min_param_ficon, Settings::path_f0, is_active_f0, ok_file_color, def_color_icon, 0);
			DrawOnesFileCkUnit(
				f_unit_zone_rgt, pad_wh_icon, spase_path_h, PX_path_pad,
				min_param_ficon, Settings::path_f1, is_active_f1, ok_file_color, def_color_icon, 1);

			if (UI_Obj::BTN_Image(SwapPathBTNz, Resources::GetTX_reload(), BLUE, 0.15f)) {
				std::swap(Settings::path_f0, Settings::path_f1);
			}
	
			C(f_unit_zone_lft);
			C(f_unit_zone_rgt);
		}
	}

//       ___              _         _  _              ___ _ _     
//      / __|_ _ ___ __ _| |_ ___  | \| |_____ __ __ | __(_) |___ 
//     | (__| '_/ -_) _` |  _/ -_) | .` / -_) V  V / | _|| | / -_)
//      \___|_| \___\__,_|\__\___| |_|\_\___|\_/\_/  |_| |_|_\___|
// ----------------------------------------------------------------------------------
	inline void DrawEnterData2CreateNew(Rectangle aa, Rectangle file_zone) {
		
		float rdef_text_h = UI::GetTextSize();
		float h_new = rdef_text_h * new_txt_multip;
		float pad_y = aa.height * makenew_element_padding;
		
		Theme::SS_sst_makenew(h_new);
		GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(Const::Colors::TXT_def));
		
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// NAME
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		float LR_padding = file_zone.width * mpass_pad_lr_btns;
		Rectangle r_f_name
		{
			file_zone.x + (file_zone.width * 0.5f) - (aa.width * makenew_name_x1) * 0.5f,
			file_zone.y + pad_y,
			aa.width * makenew_name_x1,
			aa.height * makenew_name_y1
		};

		Theme::SS_sst_XX();
		if (UI_Obj::BTN_X16(r_f_name.x, r_f_name.y, 3)) Core::ClearNEW_NAME();

		GuiSetStyle(DEFAULT, TEXT_SIZE, h_new);
		Vector2 t_name = MeasureTextEx(GuiGetFont(), loc::t.Sst.newf.name, h_new, 1);
		DrawTextEx(GuiGetFont(), loc::t.Sst.newf.name,
			{
				r_f_name.x - t_name.x - LR_padding,
				r_f_name.y + (r_f_name.height) * 0.5f - h_new * 0.5f
			},
			h_new, 1,
			GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
		);
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
		GuiTextBox(
			r_f_name,
			Core::NEW_NAME, Const::MAX_NEW_NAME_LEN + 1, is_name_select
		) ? is_name_select = !is_name_select : is_name_select = is_name_select;
	
		snprintf(DOTlwrsTXT, sizeof(DOTlwrsTXT), ".%s", Const::LOSWORS_FILE_FORMAT);
		DrawTextEx(GuiGetFont(), DOTlwrsTXT,
			{
				r_f_name.x + r_f_name.width + LR_padding,
				r_f_name.y + (r_f_name.height) * 0.5f - h_new * 0.5f
			},
			h_new, 1,
			GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
		);
	
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// PIN
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		Vector2 t_pin = MeasureTextEx(GuiGetFont(), "PIN:", h_new, 1);
		float p_size = r_f_name.height;
		Rectangle r_f_pass
		{
			file_zone.x + (file_zone.width * 0.5f) - p_size * 2,
			r_f_name.y + r_f_name.height + pad_y * 1.5f,
			p_size,
			p_size
		};
		DrawTextEx(GuiGetFont(), "PIN:",
			{
				r_f_pass.x - LR_padding - t_pin.x,
				r_f_pass.y + r_f_pass.height * 0.5f - t_pin.y * 0.5f
			},
			h_new, 1,
			GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
		);

		Rectangle PINzone = {
			r_f_pass.x, r_f_pass.y, (p_size * 4), p_size
		};
		UI_Obj::InputPIN(PINzone, active_PIN_ID, Core::NEW_PIN, is_pin_hide);

		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		UI_Obj::BTN_VizEye_Slider({
				r_f_pass.x + (r_f_pass.width * 4) + LR_padding,
				r_f_pass.y + (r_f_pass.height * 0.5f)
			}, { 0.0f, 0.5f }, std::round(aa.height * mpass_HdSw_y1), is_pin_hide);
	
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// PATH
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		Rectangle GPATHz = {
			file_zone.x + (file_zone.width - file_zone.width * makenew_path_x1) * 0.5f,
			PINzone.y + PINzone.height + pad_y,
			file_zone.width * makenew_path_x1,
			file_zone.height - (PINzone.y - file_zone.y + PINzone.height + pad_y)
		};
		C(GPATHz);
		float onepathH = std::round(GPATHz.height * 0.5f);
		float y_tomin = 0.8f;

		if (!Core::IS_2_FILE_TEMP) {
			GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			Rectangle pos0 { GPATHz.x, GPATHz.y + (GPATHz.height - onepathH) * 0.5f, GPATHz.width, onepathH * y_tomin };
			UI_Obj::PATH_DIR(pos0, Core::NEW_PATH_0, sizeof(Core::NEW_PATH_0), is_new_active_f0, Core::ClearNEW_PATH_0, 0);
		}
		else {
			GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			Rectangle pos0 { GPATHz.x, GPATHz.y, GPATHz.width, onepathH * y_tomin };
			Rectangle pos1 { pos0.x, pos0.y + onepathH, pos0.width, pos0.height };
			UI_Obj::PATH_DIR(pos0, Core::NEW_PATH_0, sizeof(Core::NEW_PATH_0), is_new_active_f0, Core::ClearNEW_PATH_0, 0);
			UI_Obj::PATH_DIR(pos1, Core::NEW_PATH_1, sizeof(Core::NEW_PATH_1), is_new_active_f1, Core::ClearNEW_PATH_1, 1);
		}
	}

//      ___              ___                _   
//     | _ \__ _ ______ |_ _|_ _  _ __ _  _| |_ 
//     |  _/ _` (_-<_-<  | || ' \| '_ \ || |  _|
//     |_| \__,_/__/__/ |___|_||_| .__/\_,_|\__|
// ----------------------------- |_| ------------------------------------------------
	inline bool IS_CAN_ENTER_TO_NEXT() {
		if (is_active_f0 || is_active_f1) {
			return false;
		}
		return true;
	}
	inline bool Draw_PASS_Input(Rectangle aa) {
		// delete verif h txt
		float h_txt = UI::GetTextSize();
		float pad = aa.width * mpass_pad_lr_btns;

		Theme::SS_sst_passinput(h_txt);
	
		float input_bar_x0 = (aa.width - (aa.width * mpass_txtbar_x1)) * 0.5f; // px
	
		Vector2 HdSw_pos = {
			aa.x + input_bar_x0 - pad,
			aa.y + (aa.height * mpass_txtbar_y0) + (aa.height * mpass_txtbar_y1) * 0.5f
		};

		float HdSw_y1 = std::round(mpass_HdSw_y1 * aa.height);
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		UI_Obj::BTN_VizEye_Slider(HdSw_pos, { 1.0f, 0.5f }, HdSw_y1, is_hide_MPASS);
	
		DrawTextEx(Resources::GetFT_main_font(), loc::t.Sst.passz.pass,
			{
				aa.x + input_bar_x0,
				aa.y + aa.height * mpass_txtbar_y0 - h_txt - 1
			},
			h_txt, 1, RED);
	
		// litl big text
		GuiSetStyle(DEFAULT, TEXT_SIZE, h_txt * mpass_txt_multiply);
		
		Rectangle PASSz = { aa.x + input_bar_x0, aa.y + aa.height * mpass_txtbar_y0,
				aa.width * mpass_txtbar_x1, aa.height * mpass_txtbar_y1 };

		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
		UI_Obj::Secure_Input(PASSz, Core::MASTER_PASS, Const::MASTER_PASS_SIZE + 1,
			is_active_MPASS, is_hide_MPASS);

		UI_Obj::DrawTextCenter(Resources::GetFT_main_font(), loc::t.Sst.passz.passexpln,
			{ PASSz.x, PASSz.y + PASSz.height, PASSz.width, h_txt * 0.9f}, h_txt * 0.9f, 1.0f, GRAY);
	
		GuiSetStyle(DEFAULT, TEXT_SIZE, h_txt); // set def size text
	
		Rectangle r
		{
			aa.x + input_bar_x0 + (mpass_txtbar_x1 * aa.width) + pad,
			aa.y + (mpass_txtbar_y0 * aa.height),
			mpass_txtbar_y1 * aa.height,
			mpass_txtbar_y1 * aa.height
		};
	
		if (UI_Obj::BTN_Image(
			{ r.x,r.y,r.width,r.height },
			Resources::GetTX_enter(),
			GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED)), 0.2f
		) || (IsKeyPressed(KEY_ENTER) && IS_CAN_ENTER_TO_NEXT()))
		{
			if (Core::Try_ENTER()) {
				ResetAllStatus();
				return true;
			}
		}
		return false;
	}

	inline bool IS_CAN_ENTER_EXIT_MAKENEW() {
		if (is_name_select ||
			(active_PIN_ID != -1) ||
			is_new_active_f0 || is_new_active_f1 ||
			is_active_enterpass || is_active_confirmpass) {
			return false;
		}
		return true;
	}
	inline void Draw_Enter_Confirm_PASS(Rectangle aa) {
	
		float pad = aa.width * mpass_pad_lr_btns;
		float h_txt = UI::GetTextSize();
		float input_bar_x0 = (aa.width - aa.width * mpass_txtbar_x1) * 0.5f;

		Theme::SS_sst_makenew(h_txt);
	
		Rectangle zone_enter = {
			input_bar_x0,
			aa.y + aa.height * enter_pass_y0,
			aa.width * mpass_txtbar_x1,
			aa.height * mpass_txtbar_y1
		};
		Rectangle zone_confirm = {
			input_bar_x0,
			zone_enter.y + zone_enter.height + (aa.height * y_between_enterconfirm),
			zone_enter.width,
			zone_enter.height
		};
		Rectangle zone_enter_btn = {
			input_bar_x0 + zone_confirm.width + pad,
			zone_confirm.y,
			zone_confirm.height,
			zone_confirm.height
		};

		float HdSw_side = std::round(mpass_HdSw_y1 * aa.height);
		Vector2 pos_0_HdSw = {
			zone_enter.x - pad,
			std::round(zone_enter.y + (zone_enter.height * 0.5f))
		};
		Vector2 pos_1_HdSw = {
			zone_confirm.x - pad,
			std::round(zone_confirm.y + (zone_confirm.height * 0.5f))
		};
	
	
		// draw
	
		// Should be placed above all input fields to allow data entry separately
		if (UI_Obj::BTN_Image(
			zone_enter_btn,
			Resources::GetTX_enter(),
			GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED)), 0.2f
		) || (IsKeyPressed(KEY_ENTER) && IS_CAN_ENTER_EXIT_MAKENEW()))
		{
			if (Core::Try_CREATE_NEW_FILE()) {
				ResetAllStatus();
			}
		}
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		UI_Obj::BTN_VizEye_Slider(pos_0_HdSw, { 1.0f, 0.5f }, HdSw_side, is_hide_PASS_ENTER);
		UI_Obj::BTN_VizEye_Slider(pos_1_HdSw, { 1.0f, 0.5f }, HdSw_side, is_hide_PASS_CONFIRM);
	
		DrawTextEx(GuiGetFont(), loc::t.Sst.passz.enterpass,
			{
				zone_enter.x,
				zone_enter.y - h_txt - 2
			},
			h_txt, 1, RED);
	
		// confifm
		DrawTextEx(GuiGetFont(), loc::t.Sst.passz.confirmpass,
			{
				zone_confirm.x,
				zone_confirm.y - h_txt - 2
			},
			h_txt, 1, RED);
	
		GuiSetStyle(DEFAULT, TEXT_SIZE, h_txt * mpass_txt_multiply);
		GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
		UI_Obj::Secure_Input(
			zone_enter,
			Core::NEW_MASTER_PASS_ENTER, Const::MASTER_PASS_SIZE + 1, is_active_enterpass, is_hide_PASS_ENTER
		);
		UI_Obj::Secure_Input(
			zone_confirm,
			Core::NEW_MASTER_PASS_CONFIRM, Const::MASTER_PASS_SIZE + 1, is_active_confirmpass, is_hide_PASS_CONFIRM
		);
		GuiSetStyle(DEFAULT, TEXT_SIZE, h_txt); // set def size text
	}

//      _  _             _         
//     | || |___ __ _ __| |___ _ _ 
//     | __ / -_) _` / _` / -_) '_|
//     |_||_\___\__,_\__,_\___|_|  
// ----------------------------------------------------------------------------------
	inline void HelperOpenCreateNewState() {

		is_new_active_f0 = false;	is_new_active_f1 = false;
		active_PIN_ID = -1;
		is_name_select = true;
		is_pin_hide = true;
		is_hide_PASS_ENTER = true;		is_hide_PASS_CONFIRM = true;
		is_active_enterpass = false;	is_active_confirmpass = false;

		Core::Enter_CREATE_NEW();
	}
	inline void HelperCloseCreateNewState() {

		is_active_f0 = false; is_active_f1 = false;
		is_active_MPASS = false; is_hide_MPASS = true;

		Core::Exit_CREATE_NEW();
	}

	inline void Draw_Checks(Rectangle aa) {

		float minWH = std::min(aa.width, aa.height);
		Rectangle sett_btn_zone = Settings_PN::GetBtnZone(aa);
		
		float X_pos_px = sett_btn_zone.width + sett_btn_zone.x * 2;

		Theme::SS_sst_checkbox(minWH * chekbox_xy1 * 1.2f, (minWH * chekbox_xy1) * 0.3f);

		GuiCheckBox(
			Rectangle{
				aa.x + X_pos_px,
				aa.y + sett_btn_zone.x,
				minWH * chekbox_xy1,
				minWH * chekbox_xy1
			},
			loc::t.Sst.header.nofile, &Settings::is_no_file_exit);

		GuiCheckBox(
			Rectangle{
				aa.x + X_pos_px,
				aa.y + (sett_btn_zone.x * 2) + (minWH * chekbox_xy1),
				minWH * chekbox_xy1,
				minWH * chekbox_xy1
			},
			loc::t.Sst.header.forgetpath, &Settings::is_forget_after_exit);
	}
	inline void Draw_12f(Rectangle aa, Rectangle m_zone) {
		float h_txt = GuiGetStyle(DEFAULT, TEXT_SIZE);
		Vector2 textSize = MeasureTextEx(GuiGetFont(), loc::t.Sst.header.fcount, h_txt, 1);
		float x0_text = aa.x + (aa.width * 0.5f) - (textSize.x * 0.5f);

		Theme::SS_sst_12file(h_txt);

		DrawTextEx(GuiGetFont(), loc::t.Sst.header.fcount,
			{
				aa.x + x0_text,
				m_zone.y - (f_count_Hpad * aa.height) - (f_count_y1 * aa.height) - h_txt - 3
			},
			h_txt, 1,
			GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
		);
		
		int stat_12 = (Settings::is_2file) ? 1 : 0;
		if (Core::GetUIInsideState() == InsideUIState::S_ST_CREATE_NEW) {
			stat_12 = (Core::IS_2_FILE_TEMP) ? 1 : 0;
		}

		float x0_f_12 = aa.width * 0.5f - (f_count_x1 * aa.width) * 0.5f;
		int prev_v = stat_12;
		GuiToggleSlider(Rectangle{
			aa.x + x0_f_12,
			m_zone.y - (f_count_Hpad * aa.height) - (f_count_y1 * aa.height),
			f_count_x1 * aa.width,
			f_count_y1 * aa.height
			}, "1;2", &stat_12
		);
		if (NotifService::isHover()) stat_12 = prev_v;

		if (Core::GetUIInsideState() == InsideUIState::S_ST_CREATE_NEW) {
			Core::IS_2_FILE_TEMP = (stat_12 == 0) ? false : true;
		}
		else {
			Settings::is_2file = (stat_12 == 0) ? false : true;
		}
		
	}
	inline void Draw_new_BTN(Rectangle aa) {
		
		//GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		Theme::SS_sst_newbtn();
		
		bool is_create = (is_MakeNewFile) ? true : false;
		float txt_size_new = UI::GetTextSize() * new_btn_txt_multip;

		int colorValue = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
		Color def_g = GetColor(colorValue);
		def_g = { def_g.r, 185, def_g.b, def_g.a };

		GuiSetStyle(
			BUTTON, TEXT_COLOR_NORMAL,
			ColorToInt(is_create ? RED : def_g)
		);

		float BTN_H = std::min(aa.width, aa.height) * 0.06f;
		const char* buttonText = is_create ? loc::t.Sst.header.closebtn : loc::t.Sst.header.newbtn;
		float btnPAD = Settings_PN::GetBtnZone(aa).x;

		if (UI_Obj::BTN_Expand_By_Text({aa.x + aa.width - btnPAD, aa.y + btnPAD }, {1.0f, 0.0f},
			buttonText, Resources::GetFT_main_font(), BTN_H, 0.0f) && !NotifService::isHover()) {
			if (is_create) {
				HelperCloseCreateNewState();
			}
			else {
				HelperOpenCreateNewState();
			}
		}
		if (IsKeyPressed(KEY_ESCAPE) && is_create) {
			HelperCloseCreateNewState();
		}
	}
	inline void Header(Rectangle aa, Rectangle file_zone) {

		Settings_PN::DrawBtnEnter(aa);
		Draw_Checks(aa);
		Draw_12f(aa, file_zone);
		Draw_new_BTN(aa);

	}

};


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------
void Start_ST::Draw(Rectangle aa) {

	is_MakeNewFile = (Core::GetUIInsideState() == InsideUIState::S_ST_CREATE_NEW);

	Rectangle file_zone = {
		aa.x + m_cont_zone_x0 * aa.width,
		aa.y + m_cont_zone_y0 * aa.height,
		aa.width * m_cont_zone_x1,
		aa.height * m_cont_zone_y1
	};
	
	C(file_zone);

	/*
		The password Input must be rendered before the path entry fields are rendered
		because the check for text entry windows being active will not work when entering the following steps.
	*/
	if (!is_MakeNewFile) {
		if (Draw_PASS_Input(aa)) {
			return; // for after decode earlier return
		}
		DrawFile_select(aa, file_zone);
	}
	else {
		UI::Set_HeaderTextStatus(HeaderTextState::CREATE_NEW_FILE);
		Draw_Enter_Confirm_PASS(aa);
		DrawEnterData2CreateNew(aa, file_zone);
	}

	Header(aa, file_zone);
}