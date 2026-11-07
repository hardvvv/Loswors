#include "FileParam.hpp"
#include <raygui.h>

#include "../../CORE/lwrs/LwrsFileUnit.hpp"
#include "../../Resources/Resources.hpp"

#include "../../Localization/loc.hpp"

#include "../UI.hpp"

#include <string>
#include <cmath>


constexpr int 

	OLMS = 128, // one line max size
	ROW_COUNT = 21,

	t = 0;

constexpr float

	hpad_by_htext = 0.15f,

	text_outer_pad_wh = 0.02f,

	tt = 0;

namespace {

	char INFO_TEXT[ROW_COUNT][OLMS] =  {};
	char tempTEXT[OLMS] = {};

	Vector2 scroll = { 0,0 };
	Rectangle view = { 0,0,0,0 };
	 

	inline void AppendText(char* dest, size_t destSize, const char* src) {
		size_t len = strlen(dest);
		if (len < destSize - 1) {
			snprintf(dest + len, destSize - len, "%s", src);
		}
	}
	inline void FormatNsMs(char* outBuffer, size_t bufferSize, int64_t ns) {
		double ms = static_cast<double>(ns) / 1000000.0;
		snprintf(outBuffer, bufferSize, "%.2f ms", ms);
	}
	inline void FormatNsOnlyS(char* outBuffer, size_t bufferSize, int64_t ns) {
		double ms = static_cast<double>(ns) / 1000000.0;
		if (ms < 60000.0) {
			int totalSec = static_cast<int>(ms / 1000.0);
			int seconds = totalSec % 60;
			snprintf(outBuffer, bufferSize, "%ds", seconds);
		}
		else {
			int totalSec = static_cast<int>(ms / 1000.0);
			int minutes = totalSec / 60;
			int seconds = totalSec % 60;
			snprintf(outBuffer, bufferSize, "%dm %ds", minutes, seconds);
		}
	}

	inline void UpdateText() {
		/*
			File Version:
				1.0.0
			File Size:
				34.0 KB
			Pass Unit Count:
				34
			Time From Open:
				1m 24s
			Private Data Entropy 0-8:
				7.456

			--- File open time ---

			Total:
				1000.00 ms
			Argon2:
				700.03 ms
			Global | XOR & Suffle:
				12.05 ms
			Inside | XOR & Suffle:
				85.25 ms
		*/

		// 1. Static labels
		snprintf(INFO_TEXT[0], OLMS, loc::t.finfo.version);
		snprintf(INFO_TEXT[1], OLMS, "       %s", LwrsFileUnit::GetVersionTEXT());

		snprintf(INFO_TEXT[2], OLMS, loc::t.finfo.size);
		snprintf(INFO_TEXT[3], OLMS, "       %.1f KiB", static_cast<double>(LwrsFileUnit::GetFileSizeBYTE()) / 1024.0);

		snprintf(INFO_TEXT[4], OLMS, loc::t.finfo.PUcount);
		snprintf(INFO_TEXT[5], OLMS, "       %zu", LwrsFileUnit::GetPUCount());

		snprintf(INFO_TEXT[6], OLMS, loc::t.finfo.opentime);
		FormatNsOnlyS(tempTEXT, OLMS, LwrsFileUnit::GetNs_FromOpen());
		snprintf(INFO_TEXT[7], OLMS, "       %s", tempTEXT);

		snprintf(INFO_TEXT[8], OLMS, loc::t.finfo.entropy);
		snprintf(INFO_TEXT[9], OLMS, "       %.5f", LwrsFileUnit::Get_Entropy08());

		snprintf(INFO_TEXT[10], OLMS, "");
		snprintf(INFO_TEXT[11], OLMS, loc::t.finfo.Hdiv);
		snprintf(INFO_TEXT[12], OLMS, "");
		
		int64_t ns_argon2 = LwrsFileUnit::GetNs_Argon2();
		int64_t ns_xor = LwrsFileUnit::GetNs_XOR_MASK_Suffle();
		int64_t ns_inside = LwrsFileUnit::GetNs_InsidePU_XOR_MASK_Suffle();
		int64_t ns_total = ns_argon2 + ns_xor + ns_inside;

		snprintf(INFO_TEXT[13], OLMS, loc::t.finfo.totalsec);
		FormatNsMs(tempTEXT, OLMS, ns_total);
		snprintf(INFO_TEXT[14], OLMS, "       %s", tempTEXT);

		snprintf(INFO_TEXT[15], OLMS, "Argon2:");
		FormatNsMs(tempTEXT, OLMS, ns_argon2);
		snprintf(INFO_TEXT[16], OLMS, "       %s", tempTEXT);

		snprintf(INFO_TEXT[17], OLMS, loc::t.finfo.gxor);
		FormatNsMs(tempTEXT, OLMS, ns_xor);
		snprintf(INFO_TEXT[18], OLMS, "       %s", tempTEXT);

		snprintf(INFO_TEXT[19], OLMS, loc::t.finfo.ixor);
		FormatNsMs(tempTEXT, OLMS, ns_inside);
		snprintf(INFO_TEXT[20], OLMS, "       %s", tempTEXT);
	}


	inline void DrawHeader(Rectangle rect, const char* text, float borderWidth, float text_h, Color borderColor, Color textColor) {
		if (text == nullptr) return;

		DrawRectangleRec(rect, {0,0,0,100});
		if (borderWidth > 0.0f) {
			DrawRectangleLinesEx(rect, borderWidth, borderColor);
		}

		Font font = Resources::GetFT_main_font();

		Vector2 textSize = MeasureTextEx(font, text, text_h, 1.0f);

		Vector2 textPos = {
			rect.x + (rect.width - textSize.x) * 0.5f,
			rect.y + (rect.height - textSize.y) * 0.5f
		};

		DrawTextEx(font, text, textPos, text_h, 1.0f, textColor);
	}
}


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void FileParam::Reset() {
	scroll = { 0,0 };
	view = { 0,0,0,0 };
}

void FileParam::Draw(struct Rectangle aa) {

	UpdateText();

	float txtSize = std::round(std::min(aa.width, aa.height) * 0.09f);

	float text_max_len = 0;
	for (int i = 0; i < ROW_COUNT; ++i) {
		if (INFO_TEXT[i][0] == '\0') continue;
		float s = MeasureTextEx(Resources::GetFT_main_font(), INFO_TEXT[i], txtSize, 1).x;
		if (s > text_max_len) {
			text_max_len = s;
		}
	}

	float text_outer_pad_px = std::min(aa.width, aa.height) * text_outer_pad_wh;
	
	float h_pad = std::round(txtSize * hpad_by_htext);

	float total_height = std::round((text_outer_pad_px * 2) + (txtSize * ROW_COUNT));
	Rectangle panelContent = { 0, 0, text_max_len - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH), total_height};

	Color def = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
	int dark_offset = 15;
	Color dark_def = {
		(unsigned char)std::max(0, def.r - dark_offset),
		(unsigned char)std::max(0, def.g - dark_offset),
		(unsigned char)std::max(0, def.b - dark_offset),
		255
	};

	Rectangle rt_header = { aa.x, aa.y, aa.width, txtSize };
	Rectangle rt_scroll = { aa.x, aa.y + txtSize, aa.width, aa.height - txtSize };
	
	DrawHeader(rt_header, loc::t.finfo.header, 1.0f,
		txtSize, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)), Const::Colors::TXT_def);

	//GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);
	GuiScrollPanel(rt_scroll, nullptr, panelContent, &scroll, &view); // header exist
	//DrawRectangleRec(view, GREEN);

	BeginScissorMode(rt_scroll.x, rt_scroll.y, rt_scroll.width, rt_scroll.height);

	Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));

	for (int i = 0; i < ROW_COUNT; ++i) {
		if (INFO_TEXT[i][0] == '\0') continue;

		Vector2 textPos = {
			rt_scroll.x + text_outer_pad_px + scroll.x,
			rt_scroll.y + text_outer_pad_px + (txtSize * i) + scroll.y
		};

		if (textPos.y + txtSize >= view.y && textPos.y <= view.y + view.height) {
			DrawTextEx(Resources::GetFT_main_font(), INFO_TEXT[i], textPos, txtSize, 1, textColor);
		}
	}

	EndScissorMode();
	
	DrawRectangleLinesEx(aa, 1.0f, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));

}