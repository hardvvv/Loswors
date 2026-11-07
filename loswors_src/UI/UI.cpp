#include "UI.hpp"
#include <raygui.h>

#include <cmath>

#include <iostream>

// My
#include "../Resources/Resources.hpp"
#include "../Localization/loc.hpp"
#include "../CORE/Const.hpp"
#include "../CORE/Log.hpp"
#include "../CORE/MonoProg/MonoProg.hpp"
#include "../CORE/FS/FileSys.hpp"
#include "../CORE/Settings.hpp"

#include "Theme/Theme.hpp"
#include "NotifService.hpp"

#include "Elements/DownBar_PN.hpp"
#include "Elements/Settings_PN.hpp"
#include "Elements/Drager.hpp"

#include "States/Decode_ST.hpp"
#include "States/Intro_ST.hpp"
#include "States/Start_ST.hpp"
#include "States/Main_ST.hpp"

#define TEST_RESIZE 0

namespace {
	namespace ltxt {
		// tgr - (trigger)
		namespace tgr {
			constexpr char NoFile[]		= "[ No File Exit ] >> TRIGGER -> (Exit to Start state)";
			constexpr char TimeOut[]	= "[ Inactive Timeout ] >> TRIGGER -> (Exit to Start state)";
		}

		constexpr char WinCreate[]	= "[ UI ] >> Create Window";
		constexpr char EndInit[]	= "[ UI ] >> End INIT <<";
	}
}

//      _   _ ___                        _                
//     | | | |_ _|  _ __ ___   __ _ _ __| | ___   _ _ __  
//     | | | || |  | '_ ` _ \ / _` | '__| |/ / | | | '_ \ 
//     | |_| || |  | | | | | | (_| | |  |   <| |_| | |_) |
//      \___/|___| |_| |_| |_|\__,_|_|  |_|\_\\__,_| .__/ 
// ----------------------------------------------- |_| ---

constexpr float 
	down_bar_y1				= 0.0416f,
	header_btn_h_ref_AR		= 1.5f,
	header_ProgIcon_xy1_h	= 0.7f,

	end = 0;

namespace {

	bool isWinNeedClose = false;




	// for test only
	inline void C(Rectangle rt) {
		DrawRectangleLinesEx(rt, 1.0f, RED);
	}

	timer t_inact = timer();
	inline bool IsExitToStart() {

		// | Is No File Exit
		// |_____________________________________________________
		if (Settings::is_no_file_exit && Core::GetUI_ctx().ui_state == UI_state::Main) {
			if (Settings::is_2file) {
				if (!FileSys::Ping_FileExists(Settings::path_f0) || !FileSys::Ping_FileExists(Settings::path_f1)) {
					Log::add(ltxt::tgr::NoFile, L_WARNING);
					return true;
				}
			}
			else {
				if (!FileSys::Ping_FileExists(Settings::path_f0)) {
					Log::add(ltxt::tgr::NoFile, L_WARNING);
					return true;
				}
			}
		}

		// | Is Inactive Exit
		// |_____________________________________________________
		auto IsActiveNOW = []() -> bool {
			if (!IsWindowFocused()) return false;

			if (GetMouseDelta().x != 0.0f || GetMouseDelta().y != 0.0f) return true;
			if (GetMouseWheelMove() != 0.0f) return true;
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) ||
				IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ||
				IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) return true;

			if (GetKeyPressed() != 0) return true;

			return false;
		};

		if (IsActiveNOW()) {
			t_inact.Start();
		}
		else if (t_inact.GetTime() >= Settings::inactive_timeout_sec && Core::GetUI_ctx().ui_state == UI_state::Main) {
			t_inact.Start();
			Log::add(ltxt::tgr::TimeOut, L_WARNING);
			return true;
		}

		return false;
	}



	//      _  _             _         
	//     | || |___ __ _ __| |___ _ _ 
	//     | __ / -_) _` / _` / -_) '_|
	//     |_||_\___\__,_\__,_\___|_|  
	// -----------------------------------------------------------------
	inline void MainBtnClick() {
		DownBar_PN::Reset();

		UI::Set_FSP(FS_Panel::NONE);
		Core::EXIT_TO_START();
	}
	bool isWindDrag = false; Vector2 WinDragOffset = { 0, 0 };
	bool isp_wbtn_main	= false;
	bool isp_wbtn_minim	= false;
	bool isp_wbtn_close	= false;
	inline const char* GetHeaderText() {
		HeaderTextState s = UI::Get_HeaderTextStatus();
		switch (s)
		{
		case HeaderTextState::NONE:
			return "";
		case HeaderTextState::LOG:
			return loc::t.winheader.log;
		case HeaderTextState::VERSION:
			return loc::t.winheader.version;
		case HeaderTextState::SETTINGS:
			return loc::t.winheader.settings;
		case HeaderTextState::CREATE_NEW_FILE:
			return loc::t.winheader.crtnewfile;
		case HeaderTextState::ENTER_DATA_PASS_UNIT:
			return loc::t.winheader.editpu;
		case HeaderTextState::EDIT_FILE_SETTINGS:
			return loc::t.winheader.editfparam;
		case HeaderTextState::COPY_FILE:
			return loc::t.winheader.copyf;
		case HeaderTextState::PIN_VERIF:
			return loc::t.winheader.pinverif;
		default:
			return "";
		}
		return "";
	}
	inline void DrawArrHeaderUnit(Rectangle aa) {
		int x = static_cast<int>(aa.x) + 1;
		int y = static_cast<int>(aa.y) + 1;
		int w = static_cast<int>(aa.width) - 2;
		int h = static_cast<int>(aa.height) - 2;

		int item_size = h;
		int pair_width = item_size * 2;

		if (w < pair_width || item_size <= 0) return;
		// ceil
		int num_pairs = (w + pair_width - 1) / pair_width;

		int overlap = 0;
		if (num_pairs > 1) {
			int total_raw_width = num_pairs * pair_width;
			int excess = total_raw_width - w;
			// ceil
			overlap = (excess + (num_pairs - 2)) / (num_pairs - 1);
		}

		int current_x = x;
		for (int p = 0; p < num_pairs; ++p) {
			if (current_x + pair_width > x + w && p == num_pairs - 1) {
				current_x = x + w - pair_width; // last to right side 
			}
			Rectangle z0 = { current_x, y, item_size, item_size };
			Rectangle z1 = { current_x + item_size, y, item_size, item_size };

			UI_Obj::DrawTexture_Color(z0, Resources::GetTX_header_unit(), { 24, 27, 30, 255 });
			UI_Obj::DrawTexture_ColorRot(z1, Resources::GetTX_header_unit(), { 24, 27, 30, 255 }, 180.0f);

			// step back
			current_x += pair_width - overlap;
		}
		
		if (UI::Get_HeaderTextStatus() != HeaderTextState::NONE) {
			Vector2 headerTextsize = MeasureTextEx(Resources::GetFT_main_font(), GetHeaderText(), UI::GetTextSize(), 1);
			Vector2 textPosition = {
				aa.x + (aa.width - headerTextsize.x) * 0.5f,
				aa.y + (aa.height - headerTextsize.y) * 0.5f
			};
			float speppp = 0.1f * aa.height * 2;
			DrawRectangle(textPosition.x - speppp, y, headerTextsize.x + (speppp * 2), h, { 24, 27, 30, 128 });
			DrawTextEx(Resources::GetFT_main_font(), GetHeaderText(), textPosition, UI::GetTextSize(), 1, Const::Colors::TXT_def);
		}
		
	}
	inline void DrawHeader(Rectangle aa) {
		// header zone
		DrawRectangle(aa.x, aa.y, aa.width, aa.height, Const::Colors::BG_def);
		DrawRectangleLinesEx(aa, 1.0f, { 102, 115, 132, 255 });

		Vector2 BTNsize = { std::round(aa.height * header_btn_h_ref_AR), aa.height};

		Rectangle ExitBTNZone = { aa.x + (aa.width - BTNsize.x), aa.y, BTNsize.x, BTNsize.y };
		Rectangle MinimzBTNZone = { ExitBTNZone.x - BTNsize.x, aa.y, BTNsize.x, BTNsize.y };

		float info_h = aa.height * header_ProgIcon_xy1_h;
		float texth = info_h * 1.3f;
		float info_pad = aa.height * (1.0f - header_ProgIcon_xy1_h) * 0.5f;
		Rectangle ProgIconZone = { aa.x + info_pad, aa.y + info_pad, info_h, info_h };

		Vector2 tz_name = MeasureTextEx(Resources::GetFT_main_font(), Const::PROGRAM_NAME, texth, 1); // for btn
		float mbat_width = ((info_pad * 5) + ProgIconZone.width + tz_name.x);
		
		bool isok = UI_Obj::isPressInZ({ aa.x, aa.y, mbat_width, aa.height }, isp_wbtn_main);
		if (GuiButton({aa.x, aa.y, mbat_width, aa.height}, NULL) && isok) {
			MainBtnClick();
		}
		DrawTextEx(Resources::GetFT_main_font(), Const::PROGRAM_NAME,
			{
				(ProgIconZone.x * 2) + ProgIconZone.width,
				aa.y + (aa.height - tz_name.y) * 0.5f
			},
			texth, 1,
			Const::Colors::TXT_def
		);

		UI_Obj::DrawTexture_Color(ProgIconZone, Resources::GetTX_program_icon(), WHITE);

		isok = UI_Obj::isPressInZ(MinimzBTNZone, isp_wbtn_minim);
		if (UI_Obj::BTN_Image_TX_SQRT_CENTER(MinimzBTNZone, Resources::GetTX_win_minim(), Const::Colors::TXT_def) && isok) {
			MinimizeWindow();
		}
		isok = UI_Obj::isPressInZ(ExitBTNZone, isp_wbtn_close);
		if (UI_Obj::BTN_Image_TX_SQRT_CENTER(ExitBTNZone, Resources::GetTX_cros(), RED) && isok) {
			isWinNeedClose = true;
		}
		
		Rectangle DragZone = { aa.x + mbat_width, aa.y, MinimzBTNZone.x - mbat_width, aa.height };
		DrawArrHeaderUnit(DragZone);

		Vector2 mousePos = GetMousePosition();
		Vector2 windowPos = GetWindowPosition();

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, DragZone)) {
			isWindDrag = true;
			WinDragOffset = mousePos;
		}
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			isWindDrag = false;
		}
		if (isWindDrag) {
			Vector2 mouseScreenPos = GetWindowPosition();
			SetWindowPosition(
				(int)(windowPos.x + mousePos.x - WinDragOffset.x),
				(int)(windowPos.y + mousePos.y - WinDragOffset.y)
			);
		}
	}
}

//                 _            _       
//      _ __  _ __(_)_   ____ _| |_ ___ 
//     | '_ \| '__| \ \ / / _` | __/ _ \
//     | |_) | |  | |\ V / (_| | ||  __/
//     | .__/|_|  |_| \_/ \__,_|\__\___|
// --- |_| -----------------------------------------------

FS_Panel UI::fs_panel_status = FS_Panel::NONE;

HeaderTextState UI::header_text_status = HeaderTextState::NONE;

//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

FS_Panel UI::Get_FSP() {
	return fs_panel_status;
}
void UI::Set_FSP(FS_Panel status) {
	fs_panel_status = status;
}
void UI::Set_HeaderTextStatus(HeaderTextState status) {
	header_text_status = status;
}
HeaderTextState UI::Get_HeaderTextStatus() {
	return header_text_status;
}

float UI::GetTextSize() {
	float min_size = std::min(Core::GetUI_ctx().Width, Core::GetUI_ctx().Height);
	return Const::TEXT_H_COEF_WH * min_size;
}
float UI::GetMinWH() {
	return std::min(Core::GetUI_ctx().Width, Core::GetUI_ctx().Height);
}

void UI::ResetMain() {
	Main_ST::ResetUI();
}

void UI::DropFrameDecode(DecodeStatus status) {
	if (!Settings::is_show_decode_anim) return;

	// with no header because header logic not work from there
	UI_ctx cc = Core::GetUI_ctx();

	int drop = 20;
	if (status == DecodeStatus::Argon2) {
		drop = 1;
	}
	/*else if (status == DecodeStatus::I_XOR_Shuffle) {
		drop = 7;
	}*/


	NotifService::ClearAll();
	EndDrawing();
	for (int i = 0; i < drop; ++i) {
		BeginDrawing();
		ClearBackground(Const::Colors::BG_def);

		Decode_ST::Draw(Rectangle{ 0.0f, 0.0f, (float)cc.Width, (float)cc.Height }, status);
	
		EndDrawing();
	}
}

void UI::RenderFrame(UI_ctx ctx) {

	if (MonoProg::IsKillSelf()) {
		isWinNeedClose = true;
		return;
	}

#if TEST_RESIZE
	Core::RESIZE(GetScreenWidth(), GetScreenHeight() - Const::WIN_HEADER_PX);
#endif

	BeginDrawing();
	ClearBackground(Const::Colors::BG_def);

	// ----------------------------------------
	Rectangle aa_full{
		0.0f, 0.0f, ctx.Width, ctx.Height
	};
	Rectangle aa_header{
		0.0f, 0.0f, ctx.Width, Const::WIN_HEADER_PX
	};
	Rectangle aa{
		0.0f, aa_header.height, ctx.Width, ctx.Height - aa_header.height
	};
	Rectangle aa_db{
		0.0f,
		std::round(aa.y + aa.height - (aa.height * down_bar_y1)),
		ctx.Width,
		std::round(aa.height * down_bar_y1)
	};
	Rectangle aa_zone{
		0.0f, aa.y, aa.width, aa.height - aa_db.height
	};	

	Set_HeaderTextStatus(HeaderTextState::NONE); // inside drawers set diffrent

	bool is_db = true;
	bool is_draw_header = true;

	Theme::DrawBG(aa_full);

	if (Drager::Draw_END_Process(aa_full, (ctx.ui_state == UI_state::Start))) {
		EndDrawing();
		return;
	}

	if (fs_panel_status == FS_Panel::NONE) {

		if (ctx.ui_state == UI_state::Intro) {
			Intro_ST::Draw(aa_full);
			NotifService::ClearAll();
			is_db = false;
			is_draw_header = false;
		}
		else if (ctx.ui_state == UI_state::Start) {
			Start_ST::Draw(aa_zone);
		}
		else if (ctx.ui_state == UI_state::Main) {
			Main_ST::Draw(aa_zone);
		}
	}
	else {

		if (fs_panel_status == FS_Panel::LOG) {
			// log + db min inside
			DownBar_PN::DrawLog(aa_zone, aa_db);
			is_db = false;
		}
		else if (fs_panel_status == FS_Panel::VERSION) {
			// version + db min inside
			DownBar_PN::DrawVersion(aa_zone, aa_db);
			is_db = false;
		}
		else if (fs_panel_status == FS_Panel::SETTINGS) {
			Settings_PN::Draw(aa_zone);
		}
	}
	if (is_db) {
		DownBar_PN::DrawDownBar(aa_db);
	}

	// notif
	NotifService::Draw(aa);

	if (is_draw_header) {
		DrawHeader(aa_header);
	}

	// ----------------------------------------

	EndDrawing();
}

void UI::StartLoop() {
	
	while (!WindowShouldClose() && !isWinNeedClose) {

		if (IsExitToStart()) Core::EXIT_TO_START();

		RenderFrame(Core::GetUI_ctx());
	}
}


void UI::_INIT_() {

#if TEST_RESIZE
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
#else
	SetConfigFlags(FLAG_WINDOW_UNDECORATED);
#endif

	InitWindow(
		Core::GetUI_ctx().Width, Core::GetUI_ctx().Height,
		Const::PROGRAM_NAME
	);
	SetWindowMinSize(Const::WIDTH_MIN, Const::HEIGHT_MIN + Const::WIN_HEADER_PX);
	SetWindowMaxSize(Const::WIDTH_MAX, Const::HEIGHT_MAX + Const::WIN_HEADER_PX);
	Log::add(ltxt::WinCreate, L_INFO);

	SetTargetFPS(Const::TARGET_FPS);
	SetExitKey(0);							// NO bild-in esc -> to exit program
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);	// For ping MonoProg File & check inactive While not in context
	
	t_inact.Start();

	Resources::LoadAll();
	SetWindowIcon(Resources::GetIMG_program_icon());

	//SetTheme(); // only after InitWindow
	Theme::SetTheme(Settings::is_random_theme ? -1 : Settings::ui_theme_id);

	Log::add(ltxt::EndInit, L_INFO);
}
void UI::_UNINIT_() {

	Resources::ClearData();

	CloseWindow();

}