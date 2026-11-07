#include "PassGen.hpp"
#include <raygui.h>

#include <cmath>
#include <string>
#include <random>

#include "../../CORE/Const.hpp"
#include "../../Resources/Resources.hpp"
#include "../../Localization/loc.hpp"
#include "../NotifService.hpp"
#include "../UI_Obj.hpp"
#include "../Theme/Theme.hpp"

//      _   _ ___                  _             
//     | | | |_ _|  _ __  __ _ _ _| |___  _ _ __ 
//     | |_| || |  | '  \/ _` | '_| / / || | '_ \
//      \___/|___| |_|_|_\__,_|_| |_\_\\_,_| .__/
// --------------------------------------- |_| -----------

constexpr float
	pad_WH_main = 0.02f,
	header_btn_y1 = 0.1f,
	pad_WH_btn = 0.01f,

	min_AR = 0.7f,

	t = 0;

constexpr int 
	sliderstep = 5,
	sizeSQline = 13,

	tt = 0;

constexpr char SEQUENCE_a_z[27] = { "abcdefghijklmnopqrstuvwxyz" }; // 26 + \0
constexpr char SEQUENCE_A_Z[27] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }; // 26 + \0
constexpr char SEQUENCE_0_9[11] = { "0123456789" };					// 10 + \0
constexpr char SEQUENCE_SPC[16] = { "!@#$%^&*()-_+=?" };			// 15 + \0

constexpr char SEQUENCE_NAME[4][sizeSQline] = {
	"a-z: %d %s",
	"A-Z: %d %s",
	"0-9: %d %s",
	"@$#: %d %s"
};

namespace {

	char G_PASS[Const::PU_PASS_LEN + 1] = { "Loswors!\0" };
	bool is_pass_visible = false;

	char tempSQ[sizeSQline] = { "x-x: %d %" };

	int len_pass = 8;
	char len_txt[5] = "8\0";

	float slidercoef[4] = { 25,25,25,25 };

	bool
		isFirstEnter = true,
		btn_is_hide = false,
		is_active_pass = false,
		zone_enter_leng = false;


	void C(Rectangle rt) {
		DrawRectangleLinesEx(rt, 1.0f, RED);
	}


	std::mt19937 rng(std::random_device{}());
	inline void GEN() {
		float slider_total = 0;
		for (int i = 0; i < 4; i++) {
			slider_total += slidercoef[i];
		}
		if (slider_total == 0) {
			for (int i = 0; i < 4; i++) {
				slidercoef[i] = 25;
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				slidercoef[i] = (slidercoef[i] / slider_total) * 100.0f;
			}
		}
		// rand %%%%%%%
		int countIN[4] = { 0,0,0,0 };
		int totalCount = 0;
		for (int i = 0; i < 4; i++) {
			countIN[i] = static_cast<int>(len_pass * (slidercoef[i] / 100.0f));
			totalCount += countIN[i];
		}
		int remaining = len_pass - totalCount;
		if (remaining > 0) {
			int maxIndex = 0;
			for (int i = 1; i < 4; i++) {
				if (slidercoef[i] > slidercoef[maxIndex]) {
					maxIndex = i;
				}
			}
			countIN[maxIndex] += remaining;
		}
		int pID = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < countIN[i]; j++) {
				if (i == 0) {
					G_PASS[pID] = SEQUENCE_a_z[std::uniform_int_distribution<>(0, 25)(rng)];
					pID++;
				}
				else if (i == 1) {
					G_PASS[pID] = SEQUENCE_A_Z[std::uniform_int_distribution<>(0, 25)(rng)];
					pID++;
				}
				else if (i == 2) {
					G_PASS[pID] = SEQUENCE_0_9[std::uniform_int_distribution<>(0, 9)(rng)];
					pID++;
				}
				else if (i == 3) {
					G_PASS[pID] = SEQUENCE_SPC[std::uniform_int_distribution<>(0, 14)(rng)];
					pID++;
				}
			}
		}
		std::shuffle(G_PASS, G_PASS + pID, rng);
		G_PASS[pID] = '\0';
	}

	inline void CopyPass() {
		SetClipboardText(G_PASS);
		NotifService::AddMessage(MType::M_NORMAL, loc::t.notif.copydone0, loc::t.notif.copydone1);
	}
}


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void PassGen::Reset() {
	snprintf(G_PASS, Const::PU_PASS_LEN + 1, "%s", "Loswors!");

	is_pass_visible = false;
	len_pass = 8;
	snprintf(len_txt, 5, "%s", "8");
	slidercoef[0] = 25; slidercoef[1] = 25; slidercoef[2] = 25; slidercoef[3] = 25;

	isFirstEnter = true;
	btn_is_hide = false;
	is_active_pass = false;
	zone_enter_leng = false;
}

void PassGen::Draw(struct Rectangle aa) {
	// Theme ??

	aa = { std::round(aa.x), std::round(aa.y), std::round(aa.width), std::round(aa.height)};
	if ((aa.width / aa.height) < min_AR) {
		float centerY = aa.y + aa.height * 0.5f;
		aa.height = std::round(aa.width / min_AR);
		aa.y = std::round(centerY - (aa.height * 0.5f));
	}

	float min_WH = std::min(aa.width, aa.height);
	float PX_pad_main = std::round(min_WH * pad_WH_main);
	float PX_pad_btn = std::round(min_WH * pad_WH_btn);
	float side_btn = std::round(min_WH * header_btn_y1);

	Rectangle zone_reload = {
		aa.x + PX_pad_main,
		aa.y + PX_pad_main,
		side_btn,
		side_btn
	};
	//C(zone_reload);
	Rectangle zone_copy = {
		aa.x + aa.width - (side_btn + PX_pad_main),
		zone_reload.y,
		side_btn,
		side_btn
	};
	//C(zone_copy);
	Rectangle zone_visible = {
		zone_copy.x - side_btn - PX_pad_btn,
		zone_copy.y,
		side_btn,
		side_btn
	};
	//C(zone_visible);
	Rectangle zone_pass = {
		zone_reload.x + zone_reload.width + PX_pad_btn,
		zone_reload.y,
		(zone_visible.x - PX_pad_btn) - (zone_reload.x + zone_reload .width + PX_pad_btn),
		side_btn
	};
	//C(zone_pass);
	Rectangle zone_body = {
		zone_reload.x,
		aa.y + side_btn + PX_pad_main + PX_pad_btn,
		aa.width - PX_pad_main * 2,
		aa.height - PX_pad_main * 2 - side_btn - PX_pad_btn
	};
	//C(zone_body);

	//      ___                  
	//     |   \ _ _ __ ___ __ __
	//     | |) | '_/ _` \ V  V /
	//     |___/|_| \__,_|\_/\_/ 
	// ---------------------------------------------------

	Theme::SS_mst_generator(std::round(zone_pass.height * 0.8f));
	DrawRectangleRec(aa, Theme::C::passgen_0_rt);
	DrawRectangleLinesEx(aa, 1.0f, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));

	// header
	if (UI_Obj::BTN_Image(zone_reload, Resources::GetTX_reload(), RED, 0.16f)) {
		GEN();
		isFirstEnter = false;
	}

	GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
	if (isFirstEnter) {
		GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	}

	UI_Obj::Secure_Input(zone_pass, G_PASS, len_pass + 1, is_active_pass, btn_is_hide);

	if (is_active_pass) isFirstEnter = false;

	if (UI_Obj::BTN_Image(zone_visible, btn_is_hide ? Resources::GetTX_eye_close() : Resources::GetTX_eye_open(), RED, 0.1f)) {
		btn_is_hide = !btn_is_hide;
	}
	if (UI_Obj::BTN_Image(zone_copy, Resources::GetTX_copy(), BLUE, 0.13f)) {
		CopyPass();
	}

	// body
	float h_step = zone_body.height / 9.0f;
	float t_size = h_step - PX_pad_btn * 2;
	t_size = h_step - PX_pad_btn;

	// round step
	for (int i = 0; i < 4; i++) {
		slidercoef[i] = std::round(slidercoef[i] / sliderstep) * sliderstep;
	}

	DrawRectangleRec(zone_body, Theme::C::passgen_1_rt);
	DrawRectangleLinesEx(zone_body, 1.0f, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));

	GuiSetStyle(SLIDER, TEXT_COLOR_PRESSED, GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));

	for (int i = 0; i < 4; i++) {
		snprintf(tempSQ, sizeSQline, SEQUENCE_NAME[i], (int)slidercoef[i], "%");
		DrawTextEx(
			Resources::GetFT_main_font_mono(), tempSQ, {
				zone_body.x + PX_pad_btn * 2,
				zone_body.y + PX_pad_btn * 1.5f + (h_step * i * 2) + h_step
			},
			t_size, 1, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
		);
		GuiSliderBar(
			{
				zone_body.x + PX_pad_btn,
				zone_body.y + PX_pad_btn + (h_step * i * 2) + h_step + h_step,
				zone_body.width - PX_pad_btn * 2,
				h_step - PX_pad_btn * 2
			},
			NULL, NULL, &slidercoef[i], 0, 100
		);
	}

	DrawTextEx(
		Resources::GetFT_main_font(), loc::t.passgen.length, {
			zone_body.x + PX_pad_btn * 2,
			zone_body.y + PX_pad_btn * 2
		},
		t_size, 1, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
	);
	UI_Obj::InputInt(
		{
			zone_body.x + PX_pad_btn * 2 + MeasureTextEx(GuiGetFont(), loc::t.passgen.length, t_size, 1).x + h_step,
			zone_body.y + PX_pad_btn * 2,
			(h_step - PX_pad_btn) * 3,
			h_step - PX_pad_btn
		},
		&len_pass, len_txt, 5, &zone_enter_leng, 8, Const::PU_PASS_LEN);

}