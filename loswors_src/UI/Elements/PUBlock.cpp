#include "PUBlock.hpp"
#include <raygui.h>

#include <cmath>

#include "../NotifService.hpp"
#include "../UI_Obj.hpp"
#include "../../Resources/Resources.hpp"

#include "../../CORE/lwrs/LwrsFileUnit.hpp"

#include "../../Localization/loc.hpp"
#include "../Theme/Theme.hpp"


#define IS_RT_CHECK 0

#if IS_RT_CHECK
static inline void C(Rectangle rt) {
	DrawRectangleLinesEx(rt, 1.0f, RED);
}
#else
static inline void C(Rectangle) {}
#endif


constexpr float
	outerPad_wh = 0.07f,

	Xpad_LeftSide_wh = 0.22f,

	interactBTN_size_wh = 0.13f,

	PosInput_y1 = 0.1f,

	PadInside_wh = 0.03f,
	TextH_wh = 0.08f,
	Name_y1_wh = 0.15f,
	Pass_y1_wh = 0.18f,

	CanslOk_pad_x1 = 0.1f,
	CanslOk_y0 = 0.6f,
	CanslOk_x1 = 0.4f,
	CanslOk_y1 = 0.2f,



	t = 0;

namespace{

	struct CONTEXT
	{
		int		ACTIVE_ID = -1;

		bool	FORACT_isEditPos = false;
		int		FORACT_PosVal = 0;
		char	FORACT_PosText[16] = { "" };
		
		bool	FORACT_isPASShide = true;

		int		FORACT_desc_col = 0;
		int		FORACT_desc_row = 0;
		Vector2 FORACT_desc_scrol = { 0,0 };

		bool	FORACT_isConfirmDel = false;

		void SetActiveByUD(int ConstID) {
			ACTIVE_ID = ConstID;
			FORACT_isPASShide = true;
			FORACT_isEditPos = true;
			FORACT_isConfirmDel = false;
			FORACT_PosVal = ConstID;
			snprintf(FORACT_PosText, sizeof(FORACT_PosText), "%d", ConstID);
		}
		void SetActiveByPASSviz(int ConstID) {
			ACTIVE_ID = ConstID;
			FORACT_isPASShide = false;
			FORACT_isEditPos = false;
			FORACT_isConfirmDel = false;
			FORACT_PosVal = ConstID;
			snprintf(FORACT_PosText, sizeof(FORACT_PosText), "%d", FORACT_PosVal);
		}
		void SetActiveByDEL(int ConstID) {
			ACTIVE_ID = ConstID;

			FORACT_isConfirmDel = true;

			FORACT_isPASShide = true;
			FORACT_isEditPos = false;
			FORACT_PosVal = ConstID;
			snprintf(FORACT_PosText, sizeof(FORACT_PosText), "%d", FORACT_PosVal);
		}
		void SetActiveBySCROLL(int ConstID) {
			ACTIVE_ID = ConstID;

			FORACT_isEditPos = false;
			FORACT_isConfirmDel = false;
			FORACT_PosVal = ConstID;
			snprintf(FORACT_PosText, sizeof(FORACT_PosText), "%d", FORACT_PosVal);
		}
		void ClearActive() {
			ACTIVE_ID = -1;
			FORACT_isEditPos = false;
			FORACT_isPASShide = true;
			FORACT_isConfirmDel = false;

			FORACT_desc_col = 0;
			FORACT_desc_row = 0;
			FORACT_desc_scrol = { 0,0 };
		}

	};

	char	TEMP_PosText[16] = { "" };

	bool IS_CAST_TOUCH = true;
	bool IS_INZONE = true;

	CONTEXT CTX = CONTEXT{};
	PU tempPU = PU{};

	inline bool isACTIVEOUTSIDE(Rectangle zone) {
		bool isOutside = !CheckCollisionPointRec(GetMousePosition(), zone);

		bool hasMouseAction = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
			IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ||
			(GetMouseWheelMove() != 0.0f);

		return isOutside && hasMouseAction;
	}

	inline void CopyPass(const char* TextForCopy) {
		SetClipboardText(TextForCopy);
		NotifService::AddMessage(MType::M_NORMAL, loc::t.notif.copydone0, loc::t.notif.copydone1);
	}

	inline bool DrawConfirmDel(Rectangle aa, int ConstID) {
		Vector2 BTN_sizePX = { std::round(aa.width * CanslOk_x1), std::round(aa.height * CanslOk_y1) };
		float BTNyPX = std::round(aa.y + aa.height * CanslOk_y0);
		float padxPX = std::round(aa.width * CanslOk_pad_x1);
		
		Rectangle cnslBTNz = {
			std::round(aa.x + (aa.width - (BTN_sizePX.x * 2) - padxPX) * 0.5f), BTNyPX, BTN_sizePX.x, BTN_sizePX.y };
		Rectangle okBTNz = {
			cnslBTNz.x + cnslBTNz.width + padxPX, BTNyPX, BTN_sizePX.x, BTN_sizePX.y };

		C(cnslBTNz);
		C(okBTNz);

		if (GuiButton(cnslBTNz, loc::t.publock.canselbtn) && IS_CAST_TOUCH) {
			CTX.ClearActive();
		}
		if (GuiButton(okBTNz, loc::t.publock.okbtn) && IS_CAST_TOUCH) {
			LwrsFileUnit::DeletePU(ConstID);
			CTX.ClearActive();
			return true;
		}
		UI_Obj::DrawText_FitCenter({aa.x, aa.y, aa.width, aa.height - (aa.height - (okBTNz.y - aa.y)) },
			loc::t.publock.delquestion, Resources::GetFT_main_font(),
			GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)), 0.4f);

		return false;
	}

	inline bool DrawUD(Rectangle setposUP, int CUR_CONST_ID, Rectangle InputIntZone, Rectangle setposDOWN) {
		bool ret = false;
		int CountUnit = LwrsFileUnit::GetPUCount();
		CountUnit = (CountUnit > 0) ? --CountUnit : 0;

		// /\ ----------------------------------------------------------------------
		if (GuiButton(setposUP, nullptr) && IS_CAST_TOUCH) {
			if (LwrsFileUnit::MovePU(CUR_CONST_ID, true)) ret = true;
		}
		GuiDrawIcon(121, setposUP.x + 1, setposUP.y + 1, 1, RAYWHITE);

		// INPUT -------------------------------------------------------------------

		if (CUR_CONST_ID == CTX.ACTIVE_ID) {
			if (UI_Obj::InputInt(InputIntZone, &CTX.FORACT_PosVal,
				CTX.FORACT_PosText, sizeof(CTX.FORACT_PosText), &CTX.FORACT_isEditPos, 0, CountUnit))
			{
				if (LwrsFileUnit::MovePUtoID(CTX.ACTIVE_ID, CTX.FORACT_PosVal)) ret = true;
			}
		}
		else {
			// TEMP (one time touch)
			bool tempCastTouch = false;
			int tempINT = CUR_CONST_ID;
			snprintf(TEMP_PosText, sizeof(TEMP_PosText), "%d", CUR_CONST_ID);

			UI_Obj::InputInt(InputIntZone, &tempINT, TEMP_PosText, sizeof(TEMP_PosText), &tempCastTouch, 0, CountUnit);

			if (IS_CAST_TOUCH && CheckCollisionPointRec(GetMousePosition(), InputIntZone)) {
				CTX.SetActiveByUD(CUR_CONST_ID);
			}
		}

		// \/ ----------------------------------------------------------------------
		if (GuiButton(setposDOWN, nullptr) && IS_CAST_TOUCH) {
			if (LwrsFileUnit::MovePU(CUR_CONST_ID, false)) ret = true;
		}
		GuiDrawIcon(120, setposDOWN.x + 1, setposDOWN.y, 1, RAYWHITE);

		if (ret) CTX.ClearActive();

		return ret;
	}

	inline PU_Interact_Status Draw_BTN_INTERACT(int CUR_CONST_ID, Rectangle e_aa, Rectangle d_aa) {
		PU_Interact_Status ret = PU_Interact_Status::NONE;

		if (UI_Obj::BTN_Image(e_aa, Resources::GetTX_edit(), GREEN, 0.04f) && IS_CAST_TOUCH) {
			CTX.ClearActive();
			ret = PU_Interact_Status::ENTER_EDIT;
		}
		
		if (UI_Obj::BTN_Image(d_aa, Resources::GetTX_cros(), RED, 0.0f) && IS_CAST_TOUCH) {
			CTX.SetActiveByDEL(CUR_CONST_ID);
		}
		return ret;
	}

	inline void DrawName(Vector2 text_pos, float H_text, Rectangle infozone, const PU& tempPU) {
		DrawTextEx(Resources::GetFT_main_font(), loc::t.publock.name, text_pos, H_text, 1, Const::Colors::TXT_def);

		DrawRectangleRec(infozone, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)));

		GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		GuiSetStyle(LABEL, TEXT_SIZE, std::round(infozone.height * 0.85f));
		GuiLabel(infozone, tempPU.name);

		DrawRectangleLinesEx(infozone, 1, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));
	}
	inline void DrawPASS(Vector2 text_pos, float H_text, Rectangle laa, float pad, PU& tempPU) {
		DrawTextEx(Resources::GetFT_main_font(), loc::t.publock.password, text_pos, H_text, 1, Const::Colors::TXT_def);
		constexpr float visBTNpad_persent = 0.1f;

		float inside_pad_copy = 0.1f;
		float visBTNpad_PX = std::round(laa.height * inside_pad_copy);

		float CopyPad_PX = std::round(laa.height * inside_pad_copy);
		float SizeBtn_PX = std::round(laa.height - CopyPad_PX * 2);

		Rectangle VisBTNz = {
			laa.x + visBTNpad_PX,
			laa.y + visBTNpad_PX,
			std::round(laa.height - visBTNpad_PX * 2),
			std::round(laa.height - visBTNpad_PX * 2)
		};
		Rectangle CopyZone = {
			std::round(laa.x + laa.width - CopyPad_PX * 1.5f - SizeBtn_PX),
			laa.y + CopyPad_PX,
			SizeBtn_PX,
			SizeBtn_PX
		};
		C(CopyZone);
		Rectangle FakeInputZone = {
			VisBTNz.x + VisBTNz.width + pad,
			laa.y,
			std::round(laa.width - (laa.height + pad * 2 + VisBTNz.width)),
			laa.height
		};
		

		bool cur_isHide = false;
		if (tempPU.ConstID == CTX.ACTIVE_ID) {
			bool tempishd = CTX.FORACT_isPASShide;

			UI_Obj::BTN_VizEye(VisBTNz, RED, tempishd);

			if (IS_CAST_TOUCH) CTX.FORACT_isPASShide = tempishd;
			cur_isHide = CTX.FORACT_isPASShide;
		}
		else {
			bool tempH = true;

			UI_Obj::BTN_VizEye(VisBTNz, RED, tempH);

			if (!tempH && IS_CAST_TOUCH) {
				CTX.SetActiveByPASSviz(tempPU.ConstID);
			}
			cur_isHide = tempH;
		}

		bool noact = false;
		GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, GuiGetStyle(TEXTBOX, BORDER_COLOR_NORMAL));
		GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		GuiSetStyle(DEFAULT, TEXT_SIZE, std::round(FakeInputZone.height * 0.7f));
		UI_Obj::Secure_Input(FakeInputZone, tempPU.pass, sizeof(tempPU.pass), noact, cur_isHide);

		if (UI_Obj::Hide_BTN_Texture(CopyZone, Resources::GetTX_copy(), BLUE) && IS_INZONE) {
			CopyPass(tempPU.pass);
		}
	}
	inline void DrawDesc(Vector2 text_pos, float H_text, Rectangle infozone, PU& tempPU, Rectangle out_scissor) {
		DrawTextEx(Resources::GetFT_main_font(), loc::t.publock.description, text_pos, H_text, 1, Const::Colors::TXT_def);

		Vector2 scrl = CTX.FORACT_desc_scrol;
		bool isCurent = (tempPU.ConstID == CTX.ACTIVE_ID);
		if (!isCurent) {
			scrl = { 0,0 };
		}
		bool act = false;

		GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, GuiGetStyle(TEXTBOX, BASE_COLOR_NORMAL));
		GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, std::round(infozone.height * 0.2f));
		UI_Obj::InputMultiLine(infozone, tempPU.desc, sizeof(tempPU.desc),
			CTX.FORACT_desc_row, CTX.FORACT_desc_col, true, act, scrl, H_text, false, out_scissor);

		if (act) {
			CTX.SetActiveBySCROLL(tempPU.ConstID);
			CTX.FORACT_desc_scrol = scrl;
		}
		else if (isCurent) {
			CTX.FORACT_desc_scrol = scrl;
		}
	}
}

PU_Interact_Status PUBlock::Draw(struct Rectangle aa, float outpadPX, size_t PUfloatID, bool isCastTouch, bool is_InZone, Rectangle out_scissor) {
	IS_CAST_TOUCH = isCastTouch;
	IS_INZONE = is_InZone;
	float min_size_aa = std::min(aa.width, aa.height);
	float LeftPad_PX = std::round(min_size_aa * Xpad_LeftSide_wh);
	Rectangle insideZone = {
		std::round(aa.x + LeftPad_PX),
		std::round(aa.y),
		std::round(aa.width - LeftPad_PX),
		std::round(aa.height)
	};

	float min_size = std::min(insideZone.width, insideZone.height);
	float pad_out_px = std::round(min_size * outerPad_wh);
	float btnSize = std::round(min_size * interactBTN_size_wh);
	
	Rectangle DEL_BTNzone = {
		insideZone.x + insideZone.width - btnSize,
		insideZone.y,
		btnSize,
		btnSize
	};
	Rectangle EDIT_BTNzone = {
		DEL_BTNzone.x - btnSize,
		insideZone.y,
		btnSize,
		btnSize
	};

	Rectangle InputIntZone = {
		std::round(aa.x),
		std::round(aa.y + (aa.height - (aa.height * PosInput_y1)) * 0.5f),
		std::round(LeftPad_PX - outpadPX),
		std::round(aa.height * PosInput_y1)
	};

	float MoveBtnSize_PX = 17;
	float pad_UD = std::ceil(outpadPX * 0.5f);
	float xpp = std::round((InputIntZone.width - MoveBtnSize_PX) * 0.5f);
	Rectangle setposUP = {
		InputIntZone.x + xpp,
		InputIntZone.y - pad_UD - MoveBtnSize_PX,
		MoveBtnSize_PX,
		MoveBtnSize_PX
	};
	Rectangle setposDOWN = {
		InputIntZone.x + xpp,
		InputIntZone.y + InputIntZone.height + pad_UD,
		MoveBtnSize_PX,
		MoveBtnSize_PX
	};

	float min_s_inside = std::min(insideZone.width, insideZone.height);
	float Text_H = std::round(min_s_inside * TextH_wh);
	float NAME_H = std::round(min_s_inside * Name_y1_wh);
	float PASS_H = std::round(min_s_inside * Pass_y1_wh);
	float PadInside = std::round(min_s_inside * PadInside_wh);
	float Half_PadIn = std::round(PadInside * 0.5f);

	Vector2 NameTextPos = { insideZone.x + PadInside, insideZone.y + PadInside };
	Rectangle name_data_zone = {
		NameTextPos.x,
		NameTextPos.y + Text_H + Half_PadIn,
		insideZone.width - (NameTextPos.x - insideZone.x) - PadInside - (EDIT_BTNzone.width + DEL_BTNzone.width),
		NAME_H
	};
	
	Vector2 PassTextPos = { NameTextPos.x, name_data_zone.y + name_data_zone.height + PadInside };
	Rectangle pass_data_zone = {
		PassTextPos.x,
		PassTextPos.y + Text_H + Half_PadIn,
		std::round(insideZone.width - (PadInside * 2)),
		PASS_H
	};
	
	Vector2 DescTextPos = { NameTextPos.x, pass_data_zone.y + pass_data_zone.height + PadInside };
	Rectangle desc_data_zone = {
		PassTextPos.x,
		DescTextPos.y + Text_H + Half_PadIn,
		insideZone.width - (PadInside * 2),
		insideZone.height - Text_H * 3 - NAME_H - PASS_H - PadInside * 4 - Half_PadIn * 3
	};

	C(name_data_zone);
	C(pass_data_zone);
	C(desc_data_zone);

	C(aa);
	C(insideZone);

	Theme::SS_mst_pu_block();

	Color bgC = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)); bgC.a = 90;
	DrawRectangleRec(insideZone, bgC);
	DrawRectangleLinesEx(insideZone, 1, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));
	
	int CUR_CONST_ID = LwrsFileUnit::GetPUByfloatID(PUfloatID).ConstID;
	PU_Interact_Status ret = PU_Interact_Status::NONE;

	if (CUR_CONST_ID == CTX.ACTIVE_ID) {
		if (isACTIVEOUTSIDE(aa)) CTX.ClearActive();
	}

	bool isPOSCHANGE = DrawUD(setposUP, CUR_CONST_ID, InputIntZone, setposDOWN);

	if (CTX.FORACT_isConfirmDel && CUR_CONST_ID == CTX.ACTIVE_ID) {
		if (DrawConfirmDel(insideZone, CUR_CONST_ID)) {
			ret = PU_Interact_Status::DELETE_DONE;
		}
	}
	else {

		ret = Draw_BTN_INTERACT(CUR_CONST_ID, EDIT_BTNzone, DEL_BTNzone);

		tempPU = LwrsFileUnit::GetPUByfloatID(PUfloatID);
		DrawName(NameTextPos, Text_H, name_data_zone, tempPU);
		DrawPASS(PassTextPos, Text_H, pass_data_zone, PadInside, tempPU);
		DrawDesc(DescTextPos, Text_H, desc_data_zone, tempPU, out_scissor);
		tempPU.Clear();
	}

	if (isPOSCHANGE) return PU_Interact_Status::POS_WAS_CHANGE;
	
	return ret;
}