#include "Main_ST.hpp"
#include <raygui.h>

#include <cmath>

#include "../../CORE/SecureWipe.hpp"

#include "../../Localization/loc.hpp"
#include "../../CORE/lwrs/LwrsFileUnit.hpp"

#include "../UI.hpp"
#include "../UI_Obj.hpp"
#include "../NotifService.hpp"
#include "../Theme/Theme.hpp"

#include "../../Resources/Resources.hpp"

#include "../Elements/Settings_PN.hpp"
#include "../Elements/TimeBlock.hpp"
#include "../Elements/PassGen.hpp"
#include "../Elements/FileParam.hpp"
#include "../Elements/PUBlock.hpp"


#define RT_LINE_TEST 0

#if RT_LINE_TEST
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

// lz - (left zone)
// rz - (right zone)
constexpr float

	lz_x1			= 0.35f,
	lz_PassGen_y1	= 0.44f,


	// by local header zone
	rz_search_x1	= 0.5f,
	rz_search_y1	= 0.47f,
	rz_rBTN_y1		= 0.35f,
	rz_chekbox_xy1_wh	= 0.03f,	// wh by global zone
	rz_PUs_zone_y0		= 0.22f,

	PUZONE_AR_MAX = 1.4f,			// after that val.(where w/h) -> 2 PU in Row

	PU_AR_W_REF			= 0.5f,		// aspect ratio
	PU_inside_pad_wh	= 0.015f,	// by inside width coef

	PU_NEW_insd_y0 = 0.85f,
	PU_NEW_insd_x1 = 0.35f,
	PU_NEW_insd_y1 = 0.08f,


	PIN_zone_y0			= 0.45f,
	PIN_vizCHK_y1		= 0.06f,
	PIN_maintext_y0		= 0.13f,
	PIN_maintext_y1		= 0.135f,
	PIN_desctext_y0		= 0.29f,
	PIN_desctext_y1		= 0.065f,
	PIN_btnOuterPad_wh	= 0.15f,

	PUed_rowstepstart_y0	= 0.17f,
	PUed_rowstep_y			= 0.19f,
	PUed_headtext_y			= 0.06f,
	PUed_input_y1			= 0.09f, // name pass
	PUed_inputtext_y		= 0.06f,
	PUed_byWoutonepad		= 0.06f,
	PUed_Hsize				= 0.45f, // by PUed_input_y1
	PUed_outvizbtnpad		= 0.1f,
	PUed_downBTN_x1			= 0.25f,

	FPCP_header_div_x	= 0.72f,
	FPCP_lrbtn_x1		= 0.11f,
	FPCP_insdolddata_y1 = 0.35f,	// by btn height
	FPCP_datapad_wh		= 0.016f,
	FPCP_input_y1		= 0.07f,
	FPCP_upinputtext_y1	= 0.05f,	// by outside workzone
	FPCP_outvizbtnpad	= 0.1f,
	FPCP_textlpad_y1	= 0.02f,	// by inside O/HDz.width	
	FPCP_NewOld_x1_wh	= 0.12f,	// by outside zone
	FPCP_NewOld_y1_wh	= 0.05f,	// by outside zone

	FPCP_UPPXtext = 4.0f,		// PX pad to up for text for input field
	FPCP_padEYE = 0.1f,			// eye vizible pad in % 0-1 (by Input H)
	FPCP_tabWrefInputH = 1.5f,	// Input.heigth * FPCP_tabWrefInputH = W tab



	t = 0; // delete t  (its only for norm readinfg)

namespace {

	//      ___                        
	//     | _ \__ _ _ _ __ _ _ __  ___
	//     |  _/ _` | '_/ _` | '  \(_-<
	//     |_| \__,_|_| \__,_|_|_|_/__/
	// ----------------------------------------------------------------------------------
	char Search_Q_TXT[Const::MAX_SEARCH_TEXT + 1] = {""};
	bool is_Search = false, is_SearchDrawReset = false;
	bool is_search_name = true, is_search_desc = false;

	int PIN_ACT_ID = 0;
	int PINvizstatus = 0;

	bool isACT_EPUname = true;
	bool isACT_EPUpass = false; bool iaPassHide = true;
	bool isACT_EPUdesc = false;
	int EPUdesc_IDrow = 0; int EPUdesc_IDcol = 0;
	Vector2 EPUdesc_viewofset = { 0,0 };

	char TEMP_B_COUNT[16] = {};

	int ACT_FPRM_COPY_oPIN_ID = 0;
	bool isACT_FPRM_COPY_oPASS = false;
	int ACT_FPRM_COPY_nPIN_ID = -1;
	bool isACT_FPRM_COPY_nePASS = false;
	bool isACT_FPRM_COPY_ncPASS = false;
	bool ishd_FPRM_COPY_oPIN = true, ishd_FPRM_COPY_oPAS = true;
	bool ishd_FPRM_COPY_nPIN = true, ishd_FPRM_COPY_nePAS = true, ishd_FPRM_COPY_ncPAS = true;

	bool isACT_COPY_NEWNAME = false;
	bool isACT_COPY_F0 = false, isACT_COPY_F1 = false;


	Vector2 scrolloffset = { 0,0 };

	char DOTlwrsTXT[16] = ".lwrs";


	
	inline void DrawChecks(Rectangle right_zone, float pad, float minWH) {
		float ch_size = minWH * rz_chekbox_xy1_wh;
		float txt_size = ch_size * 1.2f;
		//GuiSetStyle(DEFAULT, TEXT_SIZE, txt_size);
		Theme::SS_mst_search_checkbox(txt_size, std::round(txt_size * 0.2f));
		DrawTextEx(GuiGetFont(), loc::t.Mst.header.search,
			{
				right_zone.x + pad * 0.5f,
				right_zone.y + pad
			},
			txt_size, 1, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
		GuiCheckBox(
			{
				right_zone.x + pad * 0.5f,
				right_zone.y + pad + txt_size + pad * 0.5f,
				ch_size,
				ch_size
			},
			loc::t.Mst.header.name, & is_search_name);

		GuiCheckBox(
			{
				right_zone.x + pad * 0.5f,
				right_zone.y + pad + txt_size + ch_size + pad,
				ch_size,
				ch_size
			},
			loc::t.Mst.header.desc, &is_search_desc);
		if (!is_search_name && !is_search_desc) {
			is_search_name = true;
		}
	}
	inline void DrawSearch(Rectangle aa, float pad) {
		Rectangle BTN_zone = {
			aa.x + aa.width - aa.height,
			aa.y,
			aa.height,
			aa.height
		};
		Rectangle SQQ_zone = {
			aa.x,
			aa.y,
			aa.width - aa.height - pad,
			aa.height
		};

		auto RESETsearch = [&]() {
			is_SearchDrawReset = false;
			scrolloffset = { 0,0 };
		};
		
		Theme::SS_mst_search(std::round(aa.height * 0.7f));

		if (UI_Obj::BTN_Image(BTN_zone, is_SearchDrawReset ? Resources::GetTX_reload() : Resources::GetTX_search(), BLUE, 0.2f) ||
			((is_Search) && IsKeyDown(KEY_ENTER))) {
			if (!NotifService::isHover()) {
				LwrsFileUnit::QQ_Search(Search_Q_TXT, is_search_name, is_search_desc);
				RESETsearch();
			}
			
		}
		(GuiTextBox(
			SQQ_zone, Search_Q_TXT, Const::MAX_SEARCH_TEXT + 1, is_Search
		) && !NotifService::isHover()) ? is_Search = !is_Search : is_Search = is_Search;

		if (UI_Obj::BTN_X16(SQQ_zone.x, SQQ_zone.y, 3) && !NotifService::isHover()) {
			Search_Q_TXT[0] = '\0';
			LwrsFileUnit::NormalizePUarr();
			RESETsearch();
		}
	}
	bool is_pres_in_PUARR = false;
	inline void DrawPUarr(Rectangle aa) {

		DrawRectangleRec(aa, { 0,0,0,30 }); // from Theme ??

		Rectangle NEWbtn = {
			std::round(aa.x + (aa.width - aa.width * PU_NEW_insd_x1) * 0.5f),
			std::round(aa.y + aa.height * PU_NEW_insd_y0),
			aa.width * PU_NEW_insd_x1, aa.height * PU_NEW_insd_y1 };

		size_t PU_COUNT = LwrsFileUnit::GetPUCount();
		if (PU_COUNT != 0) {

			bool is_2InLine = (aa.width / aa.height > PUZONE_AR_MAX);
			int ROW_COUNT = PU_COUNT;
			float SIZE_H_PU_ANYWAY = std::round(aa.width * PU_AR_W_REF);

			float NewBtnScrollPAD_y = aa.y + aa.height - (NEWbtn.y + NEWbtn.height);
			NewBtnScrollPAD_y *= 2.0f; NewBtnScrollPAD_y += NEWbtn.height;

			if (is_2InLine) {
				ROW_COUNT = (int)std::ceil((float)PU_COUNT * 0.5f);
				SIZE_H_PU_ANYWAY = std::round(aa.width * 0.5f * PU_AR_W_REF);
			}

			Rectangle size_content = { 0,0, aa.width, ROW_COUNT * SIZE_H_PU_ANYWAY + NewBtnScrollPAD_y };
			int scrolbarW = (size_content.height > aa.height) ? GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) : 0;
			size_content.width -= scrolbarW;
			size_content.height = std::max(size_content.height, aa.height);

			Rectangle view_zone = { 0 }; // inside scroll, all content inside

			Theme::SS_mst_puarr_scroll();

			GuiScrollPanel(aa, nullptr, size_content, &scrolloffset, &view_zone); // header exist

			BeginScissorMode(view_zone.x, view_zone.y, view_zone.width, view_zone.height);

			Vector2 size_unit_NOPAD = {
				(is_2InLine) ? std::floor(view_zone.width * 0.5f) : std::floor(view_zone.width),
				SIZE_H_PU_ANYWAY };

			float outpadPX = std::round(std::min(view_zone.width, view_zone.height) * PU_inside_pad_wh);
			float oph = std::round(outpadPX * 0.5f);

			Vector2 ZS = { scrolloffset.x + view_zone.x, scrolloffset.y + view_zone.y };

			Rectangle DEF_Z = { ZS.x + outpadPX, ZS.y + outpadPX,
				size_unit_NOPAD.x - outpadPX * 2, size_unit_NOPAD.y - outpadPX };
			Rectangle FIRST = { ZS.x + outpadPX, ZS.y + outpadPX,
				size_unit_NOPAD.x - outpadPX - oph,	size_unit_NOPAD.y - outpadPX };
			Rectangle SECOND = { ZS.x + size_unit_NOPAD.x + oph, ZS.y + outpadPX,
				size_unit_NOPAD.x - outpadPX - oph,	size_unit_NOPAD.y - outpadPX };

			bool isCASTTOUCH = !CheckCollisionPointRec(GetMousePosition(), NEWbtn) &&
				CheckCollisionPointRec(GetMousePosition(), aa) &&
				UI_Obj::isPressInZ({aa.x, aa.y, aa.width - scrolbarW, aa.height}, is_pres_in_PUARR);
			bool is_INZONE = !CheckCollisionPointRec(GetMousePosition(), NEWbtn) &&
				CheckCollisionPointRec(GetMousePosition(), aa);

			for (size_t i = 0; i < PU_COUNT; ++i) {

				// local aa
				Rectangle laa = { 0 };

				if (is_2InLine) {
					laa = FIRST;
					if (i % 2 == 1) {
						laa = SECOND;
					}
					laa.y += ((i / 2) * size_unit_NOPAD.y);
				}
				else {
					laa = DEF_Z;
					laa.y += (i * size_unit_NOPAD.y);
				}

				if (laa.y + laa.height < view_zone.y || laa.y > view_zone.y + view_zone.height) continue;

				PU_Interact_Status stat = PUBlock::Draw(laa, outpadPX, i, isCASTTOUCH, is_INZONE, view_zone);

				if (stat == PU_Interact_Status::POS_WAS_CHANGE) {
					is_SearchDrawReset = true;
					Core::SetSaveStatusFALSE();
				}
				else if (stat == PU_Interact_Status::ENTER_EDIT) {
					Core::Enter_EDIT_PU(i);
				}
				else if (stat == PU_Interact_Status::DELETE_DONE) {
					PU_COUNT = LwrsFileUnit::GetPUCount();
					Core::SetSaveStatusFALSE();
				}
			}
			

			EndScissorMode();
		}
		else {
			UI_Obj::DrawText_FitCenter({ aa.x, aa.y, aa.width, aa.height - (aa.height - (NEWbtn.y - aa.y))},
				loc::t.Mst.PUs.notfound, Resources::GetFT_main_font(), GRAY, 0.4f);
		}


		if (UI_Obj::BTN_Text_FitCenter(NEWbtn, loc::t.Mst.PUs.newbtn, Resources::GetFT_main_font(), 0.15f)) {
			Core::Enter_EDIT_PU(-1);
		}

		C(aa);

		DrawRectangleLinesEx({ aa.x, aa.y, aa.width, aa.height + 1 },
			1.0f, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));
	}

	bool isp_savebtn = false;
	bool isp_fparambtn = false;
	inline void DrawSaveBTN(Rectangle aa) {
		Theme::SS_mst_save_btn(!Core::GetIsSaveStatus());

		bool isok = UI_Obj::isPressInZ(aa, isp_savebtn);
		if (UI_Obj::BTN_Text_FitCenter(aa, loc::t.Mst.header.savebtn, Resources::GetFT_main_font(), 0.2f)) {
			if(isok) Core::Enter_PIN_CONFIRM_SAVE();
			return;
		}

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
			Core::Enter_PIN_CONFIRM_SAVE();
			return;
		}
	}
	inline void DrawFParamBTN(Rectangle aa) {
		Theme::SS_mst_fparam_btn();

		bool isok = UI_Obj::isPressInZ(aa, isp_fparambtn);
		if (UI_Obj::BTN_Text_FitCenter(aa, loc::t.Mst.header.fsettbtn, Resources::GetFT_main_font(), 0.2f)) {
			if (isok)  Core::Enter_CHANGE_FPARAM();
		}
	}




	
	inline void RESET_PIN_CONFIRM() {
		PIN_ACT_ID = 0;
		PINvizstatus = 0;
	}
	inline void DrawST_PIN_CONFIRM(Rectangle aa) {
		UI::Set_HeaderTextStatus(HeaderTextState::PIN_VERIF);

		bool is_Href = (aa.width > aa.height);
		float side = is_Href ? aa.height : aa.width;

		Rectangle drawzone = {
			is_Href ? (aa.width - side) * 0.5f : aa.x,
			is_Href ? aa.y : (aa.height - side) * 0.5f,
			side, side
		};
		
		float U_Side = std::round(drawzone.width * 0.166666f);
		float Pz_wigth = U_Side * 4;
		float Pz_height = U_Side;
		Rectangle PINzone = {
			std::round(drawzone.x + (drawzone.width - Pz_wigth) * 0.5f),
			std::round(drawzone.y + (drawzone.height * PIN_zone_y0)),
			Pz_wigth, Pz_height
		};
		Rectangle MTXTzone = {
			drawzone.x,
			drawzone.y + (drawzone.height * PIN_maintext_y0),
			drawzone.width,
			drawzone.height * PIN_maintext_y1
		};
		Rectangle DESCTXTzone = {
			drawzone.x,
			drawzone.y + (drawzone.height * PIN_desctext_y0),
			drawzone.width,
			drawzone.height * PIN_desctext_y1
		};

		float outp_PX = std::round(PIN_btnOuterPad_wh * U_Side);
		float u_side_pp = std::round(U_Side - outp_PX * 2);
		Rectangle EnterBtnzone = {
			std::round(PINzone.x + PINzone.width + outp_PX),
			std::round(PINzone.y + outp_PX),
			u_side_pp, u_side_pp
		};
		Rectangle ExitGzone = {
			std::round(PINzone.x - (u_side_pp + outp_PX)),
			PINzone.y,
			u_side_pp,
			U_Side
		};
		Rectangle BTNexitzone = {
			ExitGzone.x,
			ExitGzone.y,
			ExitGzone.width,
			ExitGzone.width
		};
		Rectangle TXTesczone = {
			ExitGzone.x,
			ExitGzone.y + BTNexitzone.height,
			ExitGzone.width,
			ExitGzone.height - BTNexitzone.height
		};
		Rectangle PINvizCHKzone = {
			std::round(drawzone.x + (drawzone.width * 0.5) - (drawzone.height * PIN_vizCHK_y1)),
			std::round(PINzone.y + PINzone.height + outp_PX * 2),
			std::round(drawzone.height * PIN_vizCHK_y1 * 2),
			std::round(drawzone.height * PIN_vizCHK_y1)
		};

		// DRAW ------------------------- //

		Theme::SS_mst_fp_pinverif();

		C(drawzone);
		C(PINzone);
		C(MTXTzone);
		C(DESCTXTzone);

		C(EnterBtnzone);
		C(ExitGzone);
		C(BTNexitzone);
		C(TXTesczone);

		C(PINvizCHKzone);

		UI_Obj::DrawText_FitCenter(MTXTzone, loc::t.Mst.PINconfirm.enterpin, Resources::GetFT_main_font(),
			GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)), 0.1f);
		UI_Obj::DrawText_FitCenter(DESCTXTzone, loc::t.Mst.PINconfirm.warning, Resources::GetFT_main_font(), YELLOW, 0.1f);
		
		if (UI_Obj::BTN_Image(BTNexitzone, Resources::GetTX_cros(), RED, 0.1f) ||
			IsKeyDown(KEY_ESCAPE)) {
			Core::Exit_PIN_CONFIRM_SAVE();
			RESET_PIN_CONFIRM();
		}
		UI_Obj::DrawText_FitCenter(TXTesczone, "( esc )", Resources::GetFT_main_font(), { 255,255,255,128 }, 0.1f);

		// enter btn need to be befor InputPIN (for norm KEY_ENTER logic)
		if (UI_Obj::BTN_Image(EnterBtnzone, Resources::GetTX_enter(), RED, 0.2f) ||
			(IsKeyPressed(KEY_ENTER) && PIN_ACT_ID == -1)) {
			if (Core::Try_SAVE()) {
				RESET_PIN_CONFIRM();
			}
		}

		UI_Obj::InputPIN(PINzone, PIN_ACT_ID, Core::PIN, (PINvizstatus == 0));

		GuiToggleSlider(PINvizCHKzone, "#45#;#44#", &PINvizstatus);

	}



	inline void RESET_FPRM_COPY() {
		ACT_FPRM_COPY_oPIN_ID = 0;
		isACT_FPRM_COPY_oPASS = false;
		ACT_FPRM_COPY_nPIN_ID = -1;
		isACT_FPRM_COPY_nePASS = false;
		isACT_FPRM_COPY_ncPASS = false;
		ishd_FPRM_COPY_oPIN = true; ishd_FPRM_COPY_oPAS = true;
		ishd_FPRM_COPY_nPIN = true; ishd_FPRM_COPY_nePAS = true; ishd_FPRM_COPY_ncPAS = true;

		isACT_COPY_NEWNAME = false;
		isACT_COPY_F0 = false; isACT_COPY_F1 = false;
	}

	inline void _drawEYEinplace(Rectangle bz, bool& isHide) {
		float eyepad = std::round(bz.height * FPCP_padEYE);
		float vizbtnside = bz.height - eyepad - eyepad;
		UI_Obj::BTN_VizEye({ bz.x + bz.width + eyepad, bz.y + eyepad, vizbtnside, vizbtnside }, RED, isHide);
	};
	inline bool _drawXXinplace(Rectangle bz) {
		return UI_Obj::BTN_X16(bz.x + bz.width, bz.y, 2);
	};
	inline void _drawPINTEXTinplace(Rectangle bz, const char* text, float textH, float pad) {
		Vector2 PINpos = { bz.x - MeasureTextEx(Resources::GetFT_main_font(), text, textH, 1.0f).x - pad,
				bz.y + (bz.height - textH) * 0.5f };
		DrawTextEx(Resources::GetFT_main_font(), text, PINpos, textH, 1, Theme::C::mst_fparcopy_txt);
	}


	float FPCP_minS = 1.0f;
	inline Vector2 _FPCP_draw_uperzone(Rectangle aa, bool isFPARAM, void (*COREexit)(), float InpzH, float TabInputW) {

		float half_H = std::round(aa.height * 0.5f);

		Rectangle exitBTNz = { aa.x, aa.y,std::round(aa.width * FPCP_lrbtn_x1), aa.height };
		Rectangle WARNorFPARz = { exitBTNz.x + exitBTNz.width, aa.y,
			std::round((aa.width - exitBTNz.width) * FPCP_header_div_x), half_H };
		Rectangle SWITCHz = { WARNorFPARz.x + WARNorFPARz.width, aa.y,
			aa.width - exitBTNz.width - WARNorFPARz.width, half_H };
		Rectangle OLDDATAz = {aa.x + exitBTNz.width, aa.y + half_H, aa.width - exitBTNz.width, half_H };

		float swith_pad = std::round(SWITCHz.height * 0.1538f); // 1/6.5

		// DRAW ------------------------- //

		auto drawSwitchFP2CP = [&](Rectangle zone) {

			Rectangle FPRMbtnz = { zone.x + swith_pad, zone.y + swith_pad,
				zone.width - swith_pad - swith_pad, swith_pad + swith_pad };
			Rectangle COPYbtnz = { FPRMbtnz.x, std::round(FPRMbtnz.y + FPRMbtnz.height + swith_pad * 0.5f),
				FPRMbtnz.width, FPRMbtnz.height };

			if (isFPARAM) {
				Theme::SS_mst_fp_fparcopy_btn_act();
				UI_Obj::BTN_Text_FitCenter(FPRMbtnz, loc::t.Mst.FparFcpy.fparambtn, Resources::GetFT_main_font(), 0.15f);
				Theme::SS_mst_fp_fparcopy_btn_pas();
				if (UI_Obj::BTN_Text_FitCenter(COPYbtnz, loc::t.Mst.FparFcpy.fcopybtn, Resources::GetFT_main_font(), 0.15f)) {
					Core::Enter_COPY_F();
				}
			}
			else {
				Theme::SS_mst_fp_fparcopy_btn_pas();
				if (UI_Obj::BTN_Text_FitCenter(FPRMbtnz, loc::t.Mst.FparFcpy.fparambtn, Resources::GetFT_main_font(), 0.15f)) {
					Core::Enter_CHANGE_FPARAM();
				}
				Theme::SS_mst_fp_fparcopy_btn_act();
				UI_Obj::BTN_Text_FitCenter(COPYbtnz, loc::t.Mst.FparFcpy.fcopybtn, Resources::GetFT_main_font(), 0.15f);
			}
			
		};

		auto drawWARNorFPAR = [&](Rectangle zone) {

			Font font = Resources::GetFT_main_font();

			if (isFPARAM) {
				Rectangle Mainz = { zone.x + swith_pad, zone.y + swith_pad, zone.width - swith_pad, zone.height - swith_pad * 2 };
				Rectangle z0 = { Mainz.x, Mainz.y, Mainz.width, std::round(Mainz.height * 0.5f)};
				Rectangle z1 = { z0.x, z0.y + z0.height, z0.width, z0.height };

				DrawRectangle(Mainz.x, Mainz.y, Mainz.width, Mainz.height, Theme::C::mst_fp_fpar_bg_warning);
				Color tWc = Theme::C::mst_fp_fpar_txt_warning;
				UI_Obj::DrawText_FitCenter(z0, loc::t.Mst.FparFcpy.warning0, font, tWc, 0.2f);
				UI_Obj::DrawText_FitCenter(z1, loc::t.Mst.FparFcpy.warning1, font, tWc, 0.2f);
			}
			else {
				snprintf(DOTlwrsTXT, sizeof(DOTlwrsTXT), ".%s", Const::LOSWORS_FILE_FORMAT);

				zone = { zone.x + swith_pad, zone.y, zone.width - swith_pad, zone.height };
				Rectangle fparam_z = { zone.x, zone.y, zone.width, zone.height * 0.5f };
				Rectangle dirview_z = { zone.x, zone.y + fparam_z.height, zone.width, zone.height - fparam_z.height };
				C(fparam_z); C(dirview_z);

				// name & 2 file
				UI_Obj::SetWH_minAR(5.0f, fparam_z); C(fparam_z);

				float mainH = std::min(std::round(fparam_z.width * 0.08f), fparam_z.height * 0.7f);
				float textH = std::round(mainH * 0.8f);
				float padx = std::round(mainH * 0.2f);

				Vector2 size_name = MeasureTextEx(font, loc::t.Mst.FparFcpy.fname, textH, 1);
				Vector2 nametext_pos = { fparam_z.x + padx, fparam_z.y + (fparam_z.height - textH) * 0.5f };
				Rectangle inpNAMEz = { nametext_pos.x + size_name.x + padx,  fparam_z.y + (fparam_z.height - mainH) * 0.5f,
					mainH * 3, mainH };
				Vector2 DOTlwrs = { inpNAMEz.x + inpNAMEz.width + padx, nametext_pos.y };

				float Hchkbx = std::round(inpNAMEz.height * 0.7f);
				Rectangle CheckBox_pos = {
					inpNAMEz.x + inpNAMEz.width + MeasureTextEx(font, DOTlwrsTXT, textH, 1.0f).x + padx * 7,
					std::round(inpNAMEz.y + (inpNAMEz.height - Hchkbx) * 0.5f),
					Hchkbx, Hchkbx };

				// dir
				UI_Obj::SetWH_minAR(5.7f, dirview_z); C(dirview_z);

				// draw -------
				Theme::SS_mst_fp_fparcopy_filedata(textH, std::round(Hchkbx * 0.5f));


				GuiSetStyle(DEFAULT, TEXT_SIZE, textH);
				DrawTextEx(font, loc::t.Mst.FparFcpy.fname,
					nametext_pos, textH, 1, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

				if (GuiTextBox(inpNAMEz, Core::NEW_NAME, sizeof(Core::NEW_NAME), isACT_COPY_NEWNAME))
					isACT_COPY_NEWNAME = !isACT_COPY_NEWNAME;

				DrawTextEx(font, DOTlwrsTXT, DOTlwrs, textH, 1,
					GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
				);
				GuiCheckBox(CheckBox_pos, loc::t.Mst.FparFcpy.twofile, &Core::IS_2_FILE_TEMP);


				// fz -- 
				float wpad = std::round(dirview_z.width * 0.015f);
				float hpadup = std::round(dirview_z.height * 0.05f);
				if (!Core::IS_2_FILE_TEMP) {
					Rectangle f0 = dirview_z; f0.height -= hpadup;
					UI_Obj::PATH_DIR(f0, Core::NEW_PATH_0, sizeof(Core::NEW_PATH_0), isACT_COPY_F0, Core::ClearNEW_PATH_0, 0);
				}
				else {
					Rectangle f0 = { dirview_z.x, dirview_z.y, dirview_z.width * 0.5f, dirview_z.height - hpadup };
					Rectangle f1 = { f0.x + f0.width + wpad, f0.y, f0.width, f0.height };
					C(f0); C(f1);
					UI_Obj::PATH_DIR(f0, Core::NEW_PATH_0, sizeof(Core::NEW_PATH_0), isACT_COPY_F0, Core::ClearNEW_PATH_0, 0);
					UI_Obj::PATH_DIR(f1, Core::NEW_PATH_1, sizeof(Core::NEW_PATH_1), isACT_COPY_F1, Core::ClearNEW_PATH_1, 1);
				}
			}

		};

		auto drawOLDzone = [&](Rectangle zone) -> Vector2 {
			C(zone);
			DrawRectangle(zone.x, zone.y, zone.width, zone.height, Theme::C::mst_fparcopy_bg_old);

			float TabTextInpW = std::round(zone.width * FPCP_textlpad_y1);
			float InpzW = zone.width - TabInputW - TabInputW;
			float uperTXTH = std::round(FPCP_minS * FPCP_upinputtext_y1);
			float vizbtnpad = std::round(InpzH * FPCP_outvizbtnpad);
			float vizbtnside = InpzH - vizbtnpad - vizbtnpad;
			float freespace_H = std::round((zone.height - InpzH * 2 - uperTXTH) * 0.3333f);

			Rectangle oldtxt = { zone.x + zone.width - (FPCP_minS * FPCP_NewOld_x1_wh), zone.y,
				FPCP_minS* FPCP_NewOld_x1_wh, FPCP_minS* FPCP_NewOld_y1_wh };

			float yof = zone.y + freespace_H;
			Rectangle oPINz = { std::round(zone.x + (zone.width - InpzH * 4) * 0.5f), yof,
				std::round(InpzH * 4), InpzH };
			yof += oPINz.height + freespace_H;
			Vector2 ConfOldPass_z = { zone.x + TabTextInpW, yof - FPCP_UPPXtext };
			yof += uperTXTH;
			Rectangle OPinputZ = { zone.x + TabInputW, yof, InpzW, InpzH };

			// draw ------
			Theme::SS_mst_fp_fparcopy_secretdata();

			DrawRectangleLinesEx(oldtxt, 1.0f, Theme::C::mst_fparcopy_txt_old);
			UI_Obj::DrawText_FitCenter(oldtxt, loc::t.Mst.FparFcpy.oldz, Resources::GetFT_main_font(),
				Theme::C::mst_fparcopy_txt_old, 0.15f);

			_drawPINTEXTinplace(oPINz, loc::t.Mst.FparFcpy.oldpin, uperTXTH, vizbtnpad * 3);
			UI_Obj::InputPIN(oPINz, ACT_FPRM_COPY_oPIN_ID, Core::PIN, ishd_FPRM_COPY_oPIN);
			_drawEYEinplace(oPINz, ishd_FPRM_COPY_oPIN);


			DrawTextEx(Resources::GetFT_main_font(), loc::t.Mst.FparFcpy.oldpassconf, ConfOldPass_z, uperTXTH, 1,
				Theme::C::mst_fparcopy_txt);
			GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			UI_Obj::Secure_Input(OPinputZ, Core::ENTER_OLD_MASTER_PASS, sizeof(Core::ENTER_OLD_MASTER_PASS),
				isACT_FPRM_COPY_oPASS, ishd_FPRM_COPY_oPAS);
			_drawEYEinplace(OPinputZ, ishd_FPRM_COPY_oPAS);
			if (_drawXXinplace(OPinputZ)) Core::ClearENTER_OLD_MASTER_PASS();

			return { oldtxt.width, oldtxt.height };
		};

		//  <<<
		if (UI_Obj::BTN_Image_TX_SQRT_CENTER(exitBTNz, Resources::GetTX_cros(), RED, 0.2f) || IsKeyPressed(KEY_ESCAPE)) {
			if (COREexit != nullptr) COREexit();
			RESET_FPRM_COPY();
		}
		drawWARNorFPAR(WARNorFPARz);
		drawSwitchFP2CP(SWITCHz);
		return drawOLDzone(OLDDATAz);
	}
	inline void _FPCP_draw_downzone(Rectangle aa, bool (*COREtry)(), float InpzH, float TabInputW, Vector2 oldnewsize) {
		Rectangle NEWDATAz = { aa.x, aa.y, std::round(aa.width * (1.0f - FPCP_lrbtn_x1)), aa.height };
		Rectangle enterBTNz = { NEWDATAz.x + NEWDATAz.width, aa.y,
			std::round(aa.width * FPCP_lrbtn_x1), aa.height };
		

		// DRAW ------------------------- //
		Theme::SS_mst_fp_fparcopy_secretdata();

		auto drawNEWDATA = [&](Rectangle zone) {
			DrawRectangle(zone.x, zone.y, zone.width, zone.height, Theme::C::mst_fparcopy_bg_new);

			float zoneminS = std::min(zone.width, zone.height);

			float TabTextInpW = std::round(zone.width * FPCP_textlpad_y1);
			float InpzW = zone.width - TabInputW - TabInputW;
			float uperTXTH = std::round(FPCP_minS * FPCP_upinputtext_y1);
			float vizbtnpad = std::round(InpzH * FPCP_outvizbtnpad);
			float vizbtnside = InpzH - vizbtnpad - vizbtnpad;

			float freespaceND_H = std::round((zone.height - InpzH * 3 - uperTXTH * 2) * 0.25f);

			float yof = zone.y + freespaceND_H;
			Rectangle nPINz = { std::round(zone.x + (zone.width - InpzH * 4) * 0.5f), yof,
				std::round(InpzH * 4), InpzH };
			yof += nPINz.height + freespaceND_H;
			Vector2 EntrNewPass_z = { zone.x + TabTextInpW, yof - FPCP_UPPXtext };
			yof += uperTXTH;
			Rectangle NEPinputZ = { zone.x + TabInputW, yof, InpzW, InpzH };
			yof += NEPinputZ.height + freespaceND_H;
			Vector2 ConfNewPass_z = { EntrNewPass_z.x, yof - FPCP_UPPXtext };
			yof += uperTXTH;
			Rectangle NCPinputZ = { zone.x + TabInputW, yof, InpzW, InpzH };

			Rectangle newtxt = { zone.x + zone.width - oldnewsize.x, zone.y, oldnewsize.x, oldnewsize.y };

			 // draw -----
			Color txtC = Theme::C::mst_fparcopy_txt;

			DrawRectangleLinesEx(newtxt, 1.0f, Theme::C::mst_fparcopy_txt_new);
			UI_Obj::DrawText_FitCenter(newtxt, loc::t.Mst.FparFcpy.newz, Resources::GetFT_main_font(),
				Theme::C::mst_fparcopy_txt_new, 0.15f);


			_drawPINTEXTinplace(nPINz, loc::t.Mst.FparFcpy.newpin, uperTXTH, vizbtnpad * 3);
			UI_Obj::InputPIN(nPINz, ACT_FPRM_COPY_nPIN_ID, Core::NEW_PIN, ishd_FPRM_COPY_nPIN);
			_drawEYEinplace(nPINz, ishd_FPRM_COPY_nPIN);


			DrawTextEx(Resources::GetFT_main_font(), loc::t.Mst.FparFcpy.newpassenter, EntrNewPass_z, uperTXTH, 1, txtC);
			GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			UI_Obj::Secure_Input(NEPinputZ, Core::NEW_MASTER_PASS_ENTER, sizeof(Core::NEW_MASTER_PASS_ENTER),
				isACT_FPRM_COPY_nePASS, ishd_FPRM_COPY_nePAS);
			_drawEYEinplace(NEPinputZ, ishd_FPRM_COPY_nePAS);
			if (_drawXXinplace(NEPinputZ)) Core::ClearNEW_MASTER_PASS_ENTER();


			DrawTextEx(Resources::GetFT_main_font(), loc::t.Mst.FparFcpy.newpassconf, ConfNewPass_z, uperTXTH, 1, txtC);
			GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			UI_Obj::Secure_Input(NCPinputZ, Core::NEW_MASTER_PASS_CONFIRM, sizeof(Core::NEW_MASTER_PASS_CONFIRM),
				isACT_FPRM_COPY_ncPASS, ishd_FPRM_COPY_ncPAS);
			_drawEYEinplace(NCPinputZ, ishd_FPRM_COPY_ncPAS);
			if (_drawXXinplace(NCPinputZ)) Core::ClearNEW_MASTER_PASS_CONFIRM();

		};

		drawNEWDATA(NEWDATAz);

		if (UI_Obj::BTN_Image_TX_SQRT_CENTER(enterBTNz, Resources::GetTX_enter(), GREEN, 0.35f)) {
			if (COREtry != nullptr) if (COREtry()) RESET_FPRM_COPY();
		}
	}

	inline void DrawST_CHANGE_FPARAM(Rectangle aa) {
		UI::Set_HeaderTextStatus(HeaderTextState::EDIT_FILE_SETTINGS);

		FPCP_minS = std::min(aa.width, aa.height);

		Rectangle UPERz = { aa.x, aa.y, aa.width, std::round(aa.height * 0.5f) };
		Rectangle DOWNz = { aa.x, aa.y + UPERz.height, aa.width, UPERz.height };

		float InpzH = std::round(aa.height * FPCP_input_y1);
		float TabInputW = std::round(InpzH * FPCP_tabWrefInputH);

		Vector2 oldnewsize = _FPCP_draw_uperzone(UPERz, true, Core::Exit_CHANGE_FPARAM, InpzH, TabInputW);
		_FPCP_draw_downzone(DOWNz, Core::Try_REMAKE_FILE, InpzH, TabInputW, oldnewsize);
	}
	inline void DrawST_COPY_F(Rectangle aa) {
		UI::Set_HeaderTextStatus(HeaderTextState::COPY_FILE);

		FPCP_minS = std::min(aa.width, aa.height);

		Rectangle UPERz = { aa.x, aa.y, aa.width, std::round(aa.height * 0.5f) };
		Rectangle DOWNz = { aa.x, aa.y + UPERz.height, aa.width, UPERz.height };

		float InpzH = std::round(aa.height * FPCP_input_y1);
		float TabInputW = std::round(InpzH * FPCP_tabWrefInputH);

		Vector2 oldnewsize = _FPCP_draw_uperzone(UPERz, false, Core::Exit_COPY_F, InpzH, TabInputW);
		_FPCP_draw_downzone(DOWNz, Core::Try_COPY_FILE, InpzH, TabInputW, oldnewsize);
	}


	inline void RESET_EDIT_PU() {
		iaPassHide = true;

		isACT_EPUname = true;
		isACT_EPUpass = false;
		isACT_EPUdesc = false;
		EPUdesc_IDrow = 0; EPUdesc_IDcol = 0;
		EPUdesc_viewofset = { 0,0 };
	}
	inline void DrawST_EDIT_PU(Rectangle aa) {
		UI::Set_HeaderTextStatus(HeaderTextState::ENTER_DATA_PASS_UNIT);

		float stepstart_PX		= std::round(PUed_rowstepstart_y0 * aa.height);
		float rowstep_PX		= std::round(PUed_rowstep_y * aa.height);
		float HtextHeader_PX	= std::round(PUed_headtext_y * aa.height);
		float DefInputH_PX		= std::round(PUed_input_y1 * aa.height);
		float Htextinput_PX		= std::round(PUed_inputtext_y * aa.height);
		float bytecountH		= std::round(PUed_Hsize * DefInputH_PX);
		float Goutpad			= std::round(PUed_byWoutonepad * aa.width);
		float Winput			= std::round(aa.width - Goutpad * 4);

		float TH_x = aa.x + Goutpad;
		Vector2 HeadNameSP = { TH_x, stepstart_PX};
		Vector2 HeadPassSP = { TH_x, HeadNameSP.y + rowstep_PX };
		Vector2 HeadDescSP = { TH_x, HeadPassSP.y + rowstep_PX };

		float OC_btn_w = std::round(aa.width * PUed_downBTN_x1); float OC_btn_h = std::round(DefInputH_PX * 0.9f);
		float OC_btn_y = std::round(aa.y + aa.height - OC_btn_h - OC_btn_h);
		Rectangle canslBTNzone = {
			std::round(aa.x - OC_btn_w + (aa.width - Goutpad) * 0.5f), OC_btn_y, OC_btn_w, OC_btn_h };
		Rectangle okBTNzone = {
			std::round(aa.x + (aa.width + Goutpad) * 0.5f), OC_btn_y, OC_btn_w, OC_btn_h };

		float Iz_x = aa.x + Goutpad * 2; float tdefpp = std::round(HtextHeader_PX * 1.1f);
		Rectangle nameIz = { Iz_x, HeadNameSP.y + tdefpp, Winput, DefInputH_PX };
		Rectangle passIz = { Iz_x, HeadPassSP.y + tdefpp, Winput, DefInputH_PX };
		Rectangle descIz = { Iz_x, HeadDescSP.y + tdefpp, Winput,
			std::round((canslBTNzone.y - OC_btn_h * 0.5f) - (HeadDescSP.y + tdefpp)) };
		
		float wbytecount = std::round(bytecountH * 4.5f);
		auto qalcBCz = [&](Rectangle inRT) -> Rectangle {
			return Rectangle{ inRT.x + inRT.width - wbytecount, inRT.y - bytecountH, wbytecount, bytecountH };};

		Rectangle nameBCz = qalcBCz(nameIz);
		Rectangle passBCz = qalcBCz(passIz);
		Rectangle descBCz = qalcBCz(descIz);

		int PXsClr = 16;
		auto qpz_Clr = [&](Rectangle inpt) -> Rectangle {
			return Rectangle{ inpt.x - PXsClr, inpt.y, (float)PXsClr, (float)PXsClr };};
		Rectangle nameCLCz = qpz_Clr(nameIz);
		Rectangle passCLCz = qpz_Clr(passIz);
		Rectangle descCLCz = qpz_Clr(descIz);

		float szBTN = std::round(DefInputH_PX - (DefInputH_PX * PUed_outvizbtnpad * 2));
		szBTN = std::round(std::min(szBTN, Goutpad * 1.6f)); // for not out right side
		Rectangle EYEbtnZ = {
			std::round(passIz.x + passIz.width + DefInputH_PX * PUed_outvizbtnpad),
			std::round(passIz.y + (DefInputH_PX - szBTN) * 0.5f),
			szBTN, szBTN };

		// draw //--//--//--//--//--//--//--//--//

		C(canslBTNzone);
		C(okBTNzone);
		C(nameIz);
		C(passIz);
		C(descIz);
		C(nameBCz);
		C(passBCz);
		C(descBCz);
		C(EYEbtnZ);

		Theme::SS_mst_fp_editpu(Htextinput_PX);

		// TEXT
		Color txtcolor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
		DrawTextEx(Resources::GetFT_main_font(), loc::t.Mst.editPU.name, HeadNameSP, HtextHeader_PX, 1, txtcolor);
		DrawTextEx(Resources::GetFT_main_font(), loc::t.Mst.editPU.pass, HeadPassSP, HtextHeader_PX, 1, txtcolor);
		DrawTextEx(Resources::GetFT_main_font(), loc::t.Mst.editPU.desc, HeadDescSP, HtextHeader_PX, 1, txtcolor);

		// INPUT
		GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
		bool tmpACT_NAME = isACT_EPUname;
		bool tmpACT_PASS = isACT_EPUpass;
		isACT_EPUname = GuiTextBox(nameIz, Core::curPU.name, Const::PU_NAME_LEN + 1, isACT_EPUname) ?
			!isACT_EPUname : isACT_EPUname;
		UI_Obj::Secure_Input(passIz, Core::curPU.pass, Const::PU_PASS_LEN + 1, isACT_EPUpass, iaPassHide);
		UI_Obj::InputMultiLine(descIz, Core::curPU.desc, sizeof(Core::curPU.desc), EPUdesc_IDrow, EPUdesc_IDcol,
			false, isACT_EPUdesc, EPUdesc_viewofset, HtextHeader_PX);

		// BYTE
		GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		GuiSetStyle(DEFAULT, TEXT_SIZE, bytecountH * 0.9f);
		snprintf(TEMP_B_COUNT, sizeof(TEMP_B_COUNT), "%d/%d", (int)strlen(Core::curPU.name), (int)Const::PU_NAME_LEN);
		GuiTextBox(nameBCz, TEMP_B_COUNT, sizeof(TEMP_B_COUNT), false);
		snprintf(TEMP_B_COUNT, sizeof(TEMP_B_COUNT), "%d/%d", (int)strlen(Core::curPU.pass), (int)Const::PU_PASS_LEN);
		GuiTextBox(passBCz, TEMP_B_COUNT, sizeof(TEMP_B_COUNT), false);
		snprintf(TEMP_B_COUNT, sizeof(TEMP_B_COUNT), "%d/%d", (int)strlen(Core::curPU.desc), (int)Const::PU_DESC_LEN);
		GuiTextBox(descBCz, TEMP_B_COUNT, sizeof(TEMP_B_COUNT), false);

		// BTN
		UI_Obj::BTN_VizEye(EYEbtnZ, RED, iaPassHide);
		GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		GuiSetStyle(DEFAULT, TEXT_SIZE, OC_btn_h * 0.5f);
		if (GuiButton(canslBTNzone, loc::t.Mst.editPU.closebtn) || IsKeyPressed(KEY_ESCAPE)) {
			Core::Exit_EDIT_PU();
			RESET_EDIT_PU();
		}
		if (GuiButton(okBTNzone, loc::t.Mst.editPU.okbtn) ||
			(IsKeyPressed(KEY_ENTER) && !tmpACT_NAME && !tmpACT_PASS && !isACT_EPUdesc))
		{
			Core::Exit_SAVE_EDIT_PU();
			RESET_EDIT_PU();
		}
		
		if (GuiButton(nameCLCz, "#113#")) SecureWipe(Core::curPU.name, sizeof(Core::curPU.name));
		if (GuiButton(passCLCz, "#113#")) SecureWipe(Core::curPU.pass, sizeof(Core::curPU.pass));
		if (GuiButton(descCLCz, "#113#")) SecureWipe(Core::curPU.desc, sizeof(Core::curPU.desc));
	}
};


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void Main_ST::Draw(Rectangle aa) {

	//      ____             
	//     |_  /___ _ _  ___ 
	//      / // _ \ ' \/ -_)
	//     /___\___/_||_\___|
	// ---------------------------------------------------
	
	Rectangle sbz = Settings_PN::GetBtnZone(aa);
	float pad = std::round(sbz.x - aa.x);

	Rectangle lz = {
		aa.x,
		aa.y,
		std::round(aa.width * lz_x1),
		aa.height
	};
	Rectangle rz = {
		lz.x + lz.width,
		lz.y,
		aa.width - lz.width,
		aa.height
	};
	Rectangle watch_zone = {
		std::round(lz.x + sbz.width + pad * 2),
		lz.y + pad,
		std::round(lz.width - (sbz.width + pad * 2.5f)),
		sbz.height
	};
	Rectangle PassGen_zone = {
		lz.x + pad,
		watch_zone.y + watch_zone.height + pad,
		std::round(lz.width - pad * 1.5f),
		std::round(lz_PassGen_y1 * lz.height)
	};
	Rectangle info_zone = {
		PassGen_zone.x,
		PassGen_zone.y + PassGen_zone.height + pad,
		PassGen_zone.width,
		lz.height - ((PassGen_zone.y - aa.y) + PassGen_zone.height + pad + pad)
	};
	Rectangle pass_arr_zone = {
		std::round(rz.x + pad * 0.5f),
		std::round(rz.y + rz.height * rz_PUs_zone_y0),
		std::round(rz.width - pad * 1.5f),
		std::round(rz.height - rz.height * rz_PUs_zone_y0)
	};
	Rectangle rz_header_zone = {
		std::round(rz.x + pad * 0.5f),
		rz.y + pad,
		std::round(rz.width - pad * 1.5f),
		std::round(rz.height - pass_arr_zone.height - pad * 2)
	};
	Rectangle serch_zone = {
		std::round(rz_header_zone.x + (rz_header_zone.width - rz_header_zone.width * rz_search_x1) * 0.5f),
		std::round(rz_header_zone.y + (rz_header_zone.height - rz_header_zone.height * rz_search_y1) * 0.5f),
		std::round(rz_header_zone.width * rz_search_x1),
		std::round(rz_header_zone.height * rz_search_y1)
	};

	float x_rBTN = serch_zone.x + serch_zone.width + pad + pad;
	float h_rBTN = std::round(rz_header_zone.height * rz_rBTN_y1);
	float w_rBRN = std::round(rz_header_zone.width - serch_zone.width - (serch_zone.x - rz_header_zone.x) - pad * 2);
	Rectangle saveBTN_zone = {
		x_rBTN,
		rz_header_zone.y,
		w_rBRN,
		h_rBTN
	};
	Rectangle fileparamBTN_zone = {
		x_rBTN,
		rz_header_zone.y + rz_header_zone.height - h_rBTN,
		w_rBRN,
		h_rBTN
	};
	
	
	//      ___                  
	//     |   \ _ _ __ ___ __ __
	//     | |) | '_/ _` \ V  V /
	//     |___/|_| \__,_|\_/\_/ 
	// ---------------------------------------------------

	InsideUIState iui = Core::GetUIInsideState();
	if (iui == InsideUIState::M_ST_PIN_CONFIRM) {
		DrawST_PIN_CONFIRM(aa);
		return;
	}
	else if (iui == InsideUIState::M_ST_CHANGE_FPARAM) {
		DrawST_CHANGE_FPARAM(aa);
		return;
	}
	else if (iui == InsideUIState::M_ST_COPY_F) {
		DrawST_COPY_F(aa);
		return;
	}
	else if (iui == InsideUIState::M_ST_EDIT_PU) {
		DrawST_EDIT_PU(aa);
		return;
	}



	C(lz);
	C(rz);
	C(watch_zone);
	C(PassGen_zone);
	C(info_zone);
	C(pass_arr_zone);
	C(rz_header_zone);
	C(serch_zone);
	C(saveBTN_zone);
	C(fileparamBTN_zone);



	Settings_PN::DrawBtnEnter(aa);

	TimeBlock::Draw(watch_zone);

	PassGen::Draw(PassGen_zone);
	
	Theme::SS_mst_file_info();
	FileParam::Draw(info_zone);

	DrawChecks(rz, pad, std::min(aa.width, aa.height));

	DrawSearch(serch_zone, pad);

	DrawPUarr(pass_arr_zone);

	DrawSaveBTN(saveBTN_zone);

	DrawFParamBTN(fileparamBTN_zone);
}

void Main_ST::ResetUI() {
	PassGen::Reset();
	FileParam::Reset();

	Search_Q_TXT[0] = '\0';
	is_Search = false; is_SearchDrawReset = false;
	is_search_name = true; is_search_desc = false;

	PIN_ACT_ID = 0;
	PINvizstatus = 0;

	isACT_EPUname = true;
	isACT_EPUpass = false; iaPassHide = true;
	isACT_EPUdesc = false;
	EPUdesc_IDrow = 0; EPUdesc_IDcol = 0;
	EPUdesc_viewofset = { 0,0 };

	TEMP_B_COUNT[0] = '\0';

	ACT_FPRM_COPY_oPIN_ID	= 0;
	isACT_FPRM_COPY_oPASS	= false;
	ACT_FPRM_COPY_nPIN_ID	= -1;
	isACT_FPRM_COPY_nePASS	= false;
	isACT_FPRM_COPY_ncPASS	= false;
	ishd_FPRM_COPY_oPIN		= true; ishd_FPRM_COPY_oPAS		= true;
	ishd_FPRM_COPY_nPIN		= true; ishd_FPRM_COPY_nePAS	= true; ishd_FPRM_COPY_ncPAS = true;

	isACT_COPY_NEWNAME = false;
	isACT_COPY_F0 = false; isACT_COPY_F1 = false;


	scrolloffset = { 0,0 };

}