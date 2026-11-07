#include "Settings_PN.hpp"
#include "../UI.hpp"
#include "../UI_Obj.hpp"

#include "../../Resources/Resources.hpp"

#include "../Theme/Theme.hpp"

#include "../../Localization/loc.hpp"
#include "../../Localization/LocalSys.hpp"

#include "../../CORE/Settings.hpp"
#include "../../CORE/Log.hpp"

#include <format>

//#include <algorithm>

#include <iostream>

constexpr int
	MaxLengCount = 4,

	en = 0;

constexpr float
	BtnEE_xy0_wh		= 0.02f,
	BtnEE_xy1_wh		= 0.08f,
	BtnInsideTX_pad_wh	= 0.05f,

	LangChoose_x1_wh	= 0.13f,
	LangChoose_y1_wh	= 0.07f,

	LangText_y1_wh		= 0.05f,

	hdiv = 0.43f,

	VertDivW_wh = 0.004f,
	VertDiv_Horz_x1 = 0.2f,

	THM_veiw_y1 = 0.79f,
	THM_pad_wh = 0.08f,


	SETWH_pad_wh = 0.06f,



	end = 0.0f; // delete help

namespace {

	int LCscrollIndex = 0;	bool LCdropdownOpen = false;
	char InputTimeoutTXT[16] = { "\r" };
	bool InputTimeout_ACTIVE = false;
	int CurActTheme = -1;
	bool isPresInZoneThemeChoose[5] = { false, false, false, false, false };

	int curW = -1;
	int curH = -1;

	void ClearContext() {
		LCscrollIndex = 0;
		LCdropdownOpen = false;
		InputTimeoutTXT[0] = '\r';
		InputTimeout_ACTIVE = false;
		CurActTheme = -1;

		curW = Settings::Width;
		curH = Settings::Height;
	}

	bool ppRELOAD = false; // pp - (prev. press)
	

	// for test only
	void C(Rectangle rt) {
		DrawRectangleLinesEx(rt, 1.0f, {255, 0, 0, 128});
	}

	

	bool DrawBtnEE(Rectangle aa, Texture2D texture, Color overcolor) {
		// EE - (enter exit)
		return UI_Obj::BTN_Image(aa, texture, overcolor, BtnInsideTX_pad_wh);
	}

	bool DrawBtnExit(Rectangle aa) {
		Theme::SS_setting_main_btn();
		if (DrawBtnEE(Settings_PN::GetBtnZone(aa), Resources::GetTX_cros(), RED)) {
			ClearContext();
			return true;
		}
		return false;
	}


	void DrawLangChoose(Rectangle aa) {
		float min_side = std::min(aa.width, aa.height);
		Rectangle btn_zone = {
			std::round(aa.x + aa.width - (BtnEE_xy0_wh * min_side) - (LangChoose_x1_wh * min_side)),
			std::round(aa.y + (BtnEE_xy0_wh * min_side)),
			std::round(LangChoose_x1_wh * min_side),
			std::round(LangChoose_y1_wh * min_side)
		};
		//C(DrawLangChoose);
		float h_text_lang = LangText_y1_wh * min_side;
		Font font = Resources::GetFT_main_font();
		Vector2 tz_localiz = MeasureTextEx(font, loc::t.settings.selectlang, h_text_lang, 1);
		DrawTextEx(font, loc::t.settings.selectlang,
			{
				btn_zone.x - tz_localiz.x - (BtnEE_xy0_wh * min_side),
				btn_zone.y + (btn_zone.height) * 0.5f - h_text_lang * 0.5f
			},
			h_text_lang, 1,
			Theme::C::setting_lang_txt
		);

		int currentLang = LocalSys::GetCurLangID();

		Theme::SS_setting_lang_choose_btn(h_text_lang);

		// btn
		if (GuiButton(btn_zone, LocalSys::GetCurLangName())) {
			LCdropdownOpen = !LCdropdownOpen;
		}

		if (LCdropdownOpen) {
			float hhh = btn_zone.height * std::min(LocalSys::GetLangCount(), MaxLengCount) +
				std::min(LocalSys::GetLangCount(), MaxLengCount) + 2;

			Rectangle listBounds = {
				btn_zone.x,
				btn_zone.y + btn_zone.height,
				btn_zone.width,
				hhh - 1 // -1 for normalize pading
			};

			int activeSelected = currentLang;

			// draw scroll all
			Theme::SS_setting_lang_choose_list(h_text_lang, btn_zone.height);
			GuiListView(listBounds, LocalSys::GetAllLangName_forui(), &LCscrollIndex, &activeSelected);


			if (activeSelected != currentLang && activeSelected >= 0) {
				LocalSys::LoadLangByID(activeSelected);
				Resources::ReloadFont();
				LCdropdownOpen = false;
			}
			else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				Vector2 mousePos = GetMousePosition();
				if (!CheckCollisionPointRec(mousePos, listBounds) &&
					!CheckCollisionPointRec(mousePos, btn_zone))
				{
					LCdropdownOpen = false;
				}
			}
		}
		GuiSetStyle(DEFAULT, TEXT_SIZE, UI::GetTextSize());
	}
	
	void Draw_Checkers_END_Timeout(Rectangle aa) {
		float minS = std::min(aa.width, aa.height);
		float pad = std::round(minS * BtnEE_xy0_wh);
		float ckrPAD = std::round(pad * 0.5f);
		Rectangle MBTNz = Settings_PN::GetBtnZone(aa);

		float chkrSZstep = ((aa.height * hdiv) - (pad * 3.5f) - MBTNz.height); // available H
		chkrSZstep -= (ckrPAD * 5); // no padind
		chkrSZstep *= 0.2f; // 5 unit need to be inside
		chkrSZstep = std::round(chkrSZstep);

		float realCKRsz = chkrSZstep;
		if (aa.height > aa.width) {
			realCKRsz *= aa.width / ((aa.height == 0.0f) ? 1.0f : aa.height);
			realCKRsz = std::round(realCKRsz);
		}

		auto drawChecker = [&](const char* text, bool* val, float& yofpp) {
			GuiCheckBox({ MBTNz.x, yofpp + (chkrSZstep - realCKRsz) * 0.5f, realCKRsz, realCKRsz },
				text, val);
			yofpp += chkrSZstep + ckrPAD;
		};

		float yOFF = MBTNz.y + MBTNz.height + pad * 2;

		float textSZ = realCKRsz * 0.9f;

		Theme::SS_setting_checkbox(textSZ, ckrPAD);
		drawChecker(loc::t.settings.forgetpath, &Settings::is_forget_after_exit, yOFF);
		drawChecker(loc::t.settings.nofileexit, &Settings::is_no_file_exit, yOFF);
		drawChecker(loc::t.settings.decodeanim, &Settings::is_show_decode_anim, yOFF);
		drawChecker(loc::t.settings.randomtheme, &Settings::is_random_theme, yOFF);


		yOFF += ckrPAD;

		Vector2 tout_size = MeasureTextEx(Resources::GetFT_main_font(), loc::t.settings.inacttimeout, textSZ, 1.0f);
		
		Rectangle InputInt_z = {
			std::round(MBTNz.x + ckrPAD + tout_size.x), std::round(yOFF + (chkrSZstep - realCKRsz) * 0.5f),
			std::round(realCKRsz * 2), realCKRsz };

		// need to make text always pixel-per-pixel in the middle !!
		InputInt_z.height += (static_cast<int>(InputInt_z.height) % 2 == 0) ? 1.0f : 0.0f;

		Color ctext = Theme::C::setting_inactive_txt;
		DrawTextEx(Resources::GetFT_main_font(), loc::t.settings.inacttimeout, { MBTNz.x, yOFF + (chkrSZstep - realCKRsz) * 0.5f }, textSZ, 1.0f, ctext);
		if (InputTimeoutTXT[0] == '\r') {
			snprintf(InputTimeoutTXT, sizeof(InputTimeoutTXT), "%d", Settings::inactive_timeout_sec);
		}
		Theme::SS_setting_input_timeout(textSZ);
		UI_Obj::InputInt(InputInt_z, &Settings::inactive_timeout_sec, InputTimeoutTXT, sizeof(InputTimeoutTXT), &InputTimeout_ACTIVE,
			Const::MIN_INACTIVE_TIMEOUT_s, Const::MAX_INACTIVE_TIMEOUT_s);

		DrawTextEx(Resources::GetFT_main_font(), loc::t.settings.sec,
			{ InputInt_z.x + InputInt_z.width + ckrPAD, InputInt_z.y}, textSZ, 1.0f, ctext);
	}

	
	void DrawThemeChoose(Rectangle aa) {
		if (CurActTheme == -1) CurActTheme = Settings::ui_theme_id;

		Rectangle WorkZone = {aa.x, aa.y + (aa.height * hdiv),
			std::round(aa.width * 0.5f), std::round(aa.height * (1.0f - hdiv))};

		Rectangle view_z = { WorkZone.x, WorkZone.y, WorkZone.width, std::round(WorkZone.height * THM_veiw_y1) };
		Rectangle checkers_z = { WorkZone.x, WorkZone.y + view_z.height, WorkZone.width, WorkZone.height - view_z.height };

		float pad = std::min(view_z.width, view_z.height) * THM_pad_wh;
		view_z.x += pad; view_z.y += pad;
		view_z.width -= (pad + pad);
		view_z.height -= pad;



		float preview_window_AR = float(Const::WIDTH_DEF) / float(Const::HEIGHT_DEF);
		float window_AR = view_z.width / view_z.height;
		bool is_w_ref = (preview_window_AR >= window_AR);

		float d_w = view_z.width;
		float d_h = std::round(d_w / preview_window_AR);

		if (!is_w_ref) {
			d_h = view_z.height;
			d_w = std::round(d_h * preview_window_AR);
		}

		Rectangle PREVEIW_TX_z
		{
			std::round(view_z.x + (view_z.width - d_w) * 0.5f),
			std::round(view_z.y + (view_z.height - d_h) * 0.5f),
			d_w, d_h
		};
		
		// | PreView
		// |_______________________________________________

		//C(view_z);
		//C(checkers_z);

		Theme::DrawBG(PREVEIW_TX_z, CurActTheme, false);
		DrawRectangleLinesEx(PREVEIW_TX_z, 3, { 0,0,0,255 });

		// | Checkers !
		// |_______________________________________________

		float cbx_sz = std::round(pad * 1.2f);
		int cbxMAXid = Theme::GetThemeCount();
		float cbx_pad = std::round(cbx_sz * 0.7f);
		float start_x = checkers_z.x + (checkers_z.width - (cbx_sz * cbxMAXid + (cbx_pad * (cbxMAXid - 1)))) * 0.5f;
		
		Theme::SS_setting_theme_choose();

		for (int i = 0; i < 5; ++i) {
			Rectangle Csz = { start_x + i * (cbx_sz + cbx_pad), checkers_z.y + pad, cbx_sz, cbx_sz };

			bool isCurACT = (i == Settings::ui_theme_id);
			bool isCurTEMPACT = (i == CurActTheme);

			bool isok = UI_Obj::isPressInZ(Csz, isPresInZoneThemeChoose[i]);

			if (!isCurACT && !isCurTEMPACT) {
				// normal
				bool CastTouch = false;
				GuiCheckBox(Csz, NULL, &CastTouch);
				if (CastTouch && isok) CurActTheme = i;
			}
			else if (isCurACT) {
				// Show Choosen
				bool TEMPtrue = true;
				GuiCheckBox(Csz, NULL, &TEMPtrue);
				if (!TEMPtrue && isok) CurActTheme = i;
			}
			else if (isCurTEMPACT) {
				// Gal
				float padout = std::round(Csz.width * 0.2f);
				if (UI_Obj::BTN_Image({ Csz.x - padout, Csz.y - padout,
					Csz.width + padout + padout, Csz.height + padout + padout }, Resources::GetTX_ok_gal(), GREEN, 0.1f) && isok
				) {
					Settings::ui_theme_id = i;
					Theme::SetTheme(i);
				}
			}
		}
	}

	void DrawSetProgWH(Rectangle aa) {
		Rectangle WorkZone = { std::round(aa.x + aa.width * 0.5f), aa.y + (aa.height * hdiv),
			std::round(aa.width * 0.5f), std::round(aa.height * (1.0f - hdiv)) };

		float pad = std::round(std::min(WorkZone.width, WorkZone.height) * SETWH_pad_wh);
		float pad_out = pad * 1.5f;

		Rectangle screen_z = { WorkZone.x + pad_out, WorkZone.y + pad,
			WorkZone.width - (pad_out * 3 + pad),
			WorkZone.height - (pad * 2 + pad_out * 2) };

		Rectangle W_SLDRz = { screen_z.x, screen_z.y + screen_z.height + pad_out,
			screen_z.width, pad };
		Rectangle H_SLDRz = { screen_z.x + screen_z.width + pad_out, WorkZone.y + pad * 3,
			pad, WorkZone.height - (pad_out * 2 + pad * 4) };

		Rectangle RELOAD_BTN_z = { screen_z.x + screen_z.width + pad, screen_z.y + screen_z.height + pad,
			(pad + pad_out * 2) - pad * 2, (pad + pad_out * 2) - pad * 2 };

		Rectangle fhd_z = { screen_z.x, screen_z.y + pad * 2, screen_z.width, screen_z.height - (pad * 2) };

		constexpr float FHD_AR = 1.77777f;
		float fhdz_ar = fhd_z.width / fhd_z.height;
		bool is_w_ref = (FHD_AR > fhdz_ar);

		float FHD_h = fhd_z.height;
		float FHD_w = FHD_h * FHD_AR;
		if (is_w_ref) {
			FHD_w = fhd_z.width;
			FHD_h = FHD_w / FHD_AR;
		}

		Rectangle FULLHDz = {
			std::round(fhd_z.x + (fhd_z.width - FHD_w) * 0.5f),
			std::round(fhd_z.y + (fhd_z.height - FHD_h) * 0.5f),
			FHD_w, FHD_h };

		if (curW == -1) curW = Settings::Width;
		if (curH == -1) curH = Settings::Height;

		float W_viz = FULLHDz.width * (curW / 1920.0f);
		float H_viz = FULLHDz.height * (curH / 1080.0f);

		Rectangle viz_z = {
			FULLHDz.x + (FULLHDz.width - W_viz) * 0.5f,
			FULLHDz.y + (FULLHDz.height - H_viz) * 0.5f,
			W_viz, H_viz };

		// | PreView
		// |_______________________________________________

		auto setWH = [&](const int w, const int h) {
			Settings::Width = w;
			Settings::Height = h;
			curW = w;
			curH = h;
			Log::add(std::format("[ Settings ] >> Set window size:  {} / {}", std::to_string(w), std::to_string(h)), L_INFO);
			Core::RESIZE(w, h);
		};

		//C(WorkZone);
		//C(screen_z);

		//C(W_SLDRz);
		//C(H_SLDRz);
		//C(RELOAD_BTN_z);
		//C(FULLHDz);

		Theme::SS_setting_setdef_btn(1); // text h dont need
		if (UI_Obj::BTN_Expand_By_Text({ FULLHDz.x + FULLHDz.width * 0.5f, FULLHDz.y - pad_out },
			{ 0.5f, 0.5f }, loc::t.settings.setdefwh, Resources::GetFT_main_font(), pad_out, 0.0f)) {
			setWH(Const::WIDTH_DEF, Const::HEIGHT_DEF);
		}

		
		
		DrawRectangle(FULLHDz.x, FULLHDz.y, FULLHDz.width, FULLHDz.height, Theme::C::setting_fullhd_bg);
		DrawRectangleLinesEx(FULLHDz, 1.0f, Theme::C::setting_fullhd_txt);
		DrawRectangleLinesEx(viz_z, 1.0f, WHITE);

		DrawTextEx(Resources::GetFT_main_font(), "Full HD:",
			{ FULLHDz.x + 3, FULLHDz.y + 3 }, pad, 0.0f, Theme::C::setting_fullhd_txt);


		Theme::SS_setting_reloadsetwh_btn();
		bool isok = UI_Obj::isPressInZ(RELOAD_BTN_z, ppRELOAD); // need keep out for every frame work !!
		if (UI_Obj::BTN_Image(RELOAD_BTN_z, Resources::GetTX_reload(), BLUE, 0.15f) && isok) {
			setWH(curW, curH);
		}

		UI_Obj::SliderInt_W(H_SLDRz, &curH, Const::HEIGHT_MIN, Const::HEIGHT_MAX, 1, Resources::GetFT_main_font());
		UI_Obj::SliderInt_H(W_SLDRz, &curW, Const::WIDTH_MIN, Const::WIDTH_MAX, 1, Resources::GetFT_main_font());
	}
}	


Rectangle Settings_PN::GetBtnZone(struct Rectangle aa) {
	float min_side = std::min(aa.width, aa.height);
	return {
		aa.x + (BtnEE_xy0_wh * min_side),
		aa.y + (BtnEE_xy0_wh * min_side),
		BtnEE_xy1_wh * min_side,
		BtnEE_xy1_wh * min_side,
	};
}
void Settings_PN::DrawBtnEnter(struct Rectangle aa) {
	Theme::SS_setting_main_btn();
	if (DrawBtnEE(GetBtnZone(aa), Resources::GetTX_setting(), GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL)))) {
		UI::Set_FSP(FS_Panel::SETTINGS);
	}
}




void Settings_PN::Draw(struct Rectangle aa) {

	UI::Set_HeaderTextStatus(HeaderTextState::SETTINGS);

	if (DrawBtnExit(aa) || IsKeyPressed(KEY_ESCAPE)) {
		UI::Set_FSP(FS_Panel::NONE);
	}

	DrawRectangleRec(aa, {0,0,0,40});

	DrawLangChoose(aa);
	Draw_Checkers_END_Timeout(aa);

	DrawThemeChoose(aa);

	float thin_line = std::round(std::min(aa.width, aa.height) * VertDivW_wh);
	float lineUP_x1PX = aa.width * VertDiv_Horz_x1;
	DrawLineEx({ aa.x + aa.width * 0.5f, aa.y + aa.height * hdiv },
		{ aa.x + aa.width * 0.5f, aa.y + aa.height }, thin_line, Theme::C::setting_div_line);
	DrawLineEx({ aa.x + (aa.width - (lineUP_x1PX)) * 0.5f, aa.y + aa.height * hdiv},
		{aa.x + (aa.width + (lineUP_x1PX)) * 0.5f, aa.y + aa.height * hdiv }, thin_line, Theme::C::setting_div_line);

	DrawSetProgWH(aa);
}