#include "Intro_ST.hpp"

#include <raygui.h>

#include <format>

#include "../../CORE/Core.hpp"
#include "../../CORE/Const.hpp"
#include "../../Resources/Resources.hpp"
#include "../UI_Obj.hpp"

//      _   _ ___                        _                
//     | | | |_ _|  _ __ ___   __ _ _ __| | ___   _ _ __  
//     | | | || |  | '_ ` _ \ / _` | '__| |/ / | | | '_ \ 
//     | |_| || |  | | | | | | (_| | |  |   <| |_| | |_) |
//      \___/|___| |_| |_| |_|\__,_|_|  |_|\_\\__,_| .__/ 
// ----------------------------------------------- |_| ---

constexpr Color
	Color_ICON_BG = { 41, 44, 51, 255 },
	Color_ICON_PART = { 172, 60, 60, 255 },
	Color_BG_DARK = { 20, 23, 26, 255 };

constexpr float
	TimeStep_s			= 1.0f / Const::TARGET_FPS,
	AfterStartWait_s	= 0.2f,

	drawzone_AR			= 1.777f,


	// TP	(Time Point)		( 0 start 1 end ) where (0 = 0% | 1 = 100%) For global anim time
	// LTP	(Local Time Point)	( 0 start 1 end ) where (0 = 0% | 1 = 100%) For local anim time                                                              

	IconBGUp_TP0		= 0.00f,	IconBGUp_TP1		= 0.20f,
	IconInsideAnim_TP0	= 0.16f,	IconInsideAnim_TP1	= 0.55f,
	IconMove_TP0		= 0.55f,	IconMove_TP1		= 0.65f,

	TextAnim_TP0		= 0.57f,	TextAnim_TP1		= 0.7f,
	BG_Flade_TP0		= 0.9f,		BG_Flade_TP1		= 1.0f,

	IconXXAnim_LTP0		= 0.5f,		IconXXAnim_LTP1		= 0.8f,		//|
	IconKeyAnim_LTP0	= 0.2f,		IconKeyAnim_LTP1	= 0.84f,	//| by IconInsideAnim
	IconKHAnim_LTP0		= 0.1f,		IconKHAnim_LTP1		= 1.0f,		//|__________________

	IconKeyFlade_LTP0	= 0.0f,		IconKeyFlade_LTP1	= 0.3f,		//| by KeyKHAnim
	IconKHFlade_LTP0	= 0.0f,		IconKHFlade_LTP1	= 0.25f,	//|_____________


	// ( 0 start 1 end )

	BGScale_0			= 0.75f,	BGScale_1			= 1.0f,
	
	IconPivotX_0		= 0.5f,		IconPivotX_1		= 0.26f,
	IconPivotY_0		= 0.5f,		IconPivotY_1		= 0.5f,
	IconScale_0			= 0.95f,	IconScale_1			= 0.75f,

	IconXX_xy1_wh		= 0.243f,
	IconXXScale_0		= 0.4f,		IconXXScale_1		= 1.0f,
	IconXXRot_0			= 50.0f,	IconXXRot_1			= 0.0f,			// degr
	IconXXPivotY		= 0.19f,
	IconXX_outer_pad_x	= 0.29f,

	IconKey_xy1_wh		= 0.85f,
	IconKeyPivotX_0		= 0.292f,	IconKeyPivotX_1		= 0.486f,
	IconKeyPivotY_0		= 0.753f,	IconKeyPivotY_1		= 0.696f,

	IconKH_xy1_wh		= 0.54f,
	IconKHPivotX_1		= 0.7f,
	IconKHPivotY_1		= 0.62f,


	ProgNameText_y1		= 0.18f,
	AuthorNameText_y1	= 0.06f,

	TextProgNamePivotX	= 0.74f,	TextProgNamePivotY	= 0.42f,
	TextAutNamePivotX	= 0.74f,	TextAutNamePivotY	= 0.58f;

namespace {

	//      __  __      _   _    
	//     |  \/  |__ _| |_| |_  
	//     | |\/| / _` |  _| ' \ 
	//     |_|  |_\__,_|\__|_||_|
	// -----------------------------------------------

	float CLAMP_01(float min, float max, float val) {
		if (max == min) return 0.0f; // no zero div
		return std::max(0.0f, std::min(1.0f, (val - min) / (max - min)));
	}
	float LERP_01(float a, float b, float coef01) {
		return std::lerp(a, b, coef01);
	}
	float EaseOutBack(float t, float overshoot = 1.70158f) {
		// t = 0 <-> 1  |  overshoot 1.70158f ~10%
		float f = t - 1.0f;
		return 1.0f + (overshoot + 1.0f) * f * f * f + overshoot * f * f;
	}

	Rectangle StayOnPivotENDScale_SQRT(
		float side,
		Rectangle outer,
		float pivot_x, float pivot_y,
		float scale
	) {
		float r_side = side * scale;

		float X_piv = outer.width * pivot_x;
		float Y_piv = outer.height * pivot_y;

		float half_side = r_side / 2.0f;

		Rectangle ret{
			outer.x + (X_piv - half_side),
			outer.y + (Y_piv - half_side),
			r_side,
			r_side
		};

		return ret;
	}

	//      ___                  
	//     |   \ _ _ __ ___ __ __
	//     | |) | '_/ _` \ V  V /
	//     |___/|_| \__,_|\_/\_/ 
	// -----------------------------------------------

	// for test only
	void C(Rectangle rt) {
		DrawRectangleLinesEx(rt, 1.0f, RED);
	}

	
	void InsideIconAnim(float _01, Rectangle aa) {

		//     __  __ __  __
		//     \ \/ / \ \/ /
		//      >  <   >  < 
		//     /_/\_\ /_/\_\
		// -------------------------------------------

		float xx0PivotX = IconXX_outer_pad_x;
		float xx1PivotX = 1.0f - IconXX_outer_pad_x;
		float XXside = IconXX_xy1_wh * std::min(aa.width, aa.height);

		float XX_01 = CLAMP_01(IconXXAnim_LTP0, IconXXAnim_LTP1, _01);

		float xxScale = LERP_01(IconXXScale_0, IconXXScale_1, XX_01);
		float xxRot = LERP_01(IconXXRot_0, IconXXRot_1, XX_01);
		unsigned char xxAlpha = 255 * XX_01;

		Rectangle XX0_zone = StayOnPivotENDScale_SQRT(XXside, aa, xx0PivotX, IconXXPivotY, xxScale);
		Rectangle XX1_zone = StayOnPivotENDScale_SQRT(XXside, aa, xx1PivotX, IconXXPivotY, xxScale);

		UI_Obj::DrawTexture_ColorRot(XX0_zone, Resources::GetIntroTX_Z(),
			{ Color_ICON_PART.r, Color_ICON_PART.g, Color_ICON_PART.b, xxAlpha }, xxRot);
		UI_Obj::DrawTexture_ColorRot(XX1_zone, Resources::GetIntroTX_Z(),
			{ Color_ICON_PART.r, Color_ICON_PART.g, Color_ICON_PART.b, xxAlpha }, xxRot);

		//      _  __         
		//     | |/ /___ _  _ 
		//     | ' </ -_) || |
		//     |_|\_\___|\_, |
		// ------------- |__/ ------------------------

		float Key_01 = CLAMP_01(IconKeyAnim_LTP0, IconKeyAnim_LTP1, _01);
		float KH_01 = CLAMP_01(IconKHAnim_LTP0, IconKHAnim_LTP1, _01);
		unsigned char Key_Alpha = 255 * CLAMP_01(IconKeyFlade_LTP0, IconKeyFlade_LTP1, Key_01);
		unsigned char KH_Alpha = 255 * CLAMP_01(IconKHFlade_LTP0, IconKHFlade_LTP1, KH_01);
		
		float Key_side = IconKey_xy1_wh * std::min(aa.width, aa.height);
		float Key_pivotX = LERP_01(IconKeyPivotX_0, IconKeyPivotX_1, EaseOutBack(Key_01, 1.8f));
		float Key_pivotY = LERP_01(IconKeyPivotY_0, IconKeyPivotY_1, EaseOutBack(Key_01, 1.8f));
		float KH_side = IconKH_xy1_wh * std::min(aa.width, aa.height);

		float KH_pivotX = IconKHPivotX_1;
		float KH_pivotY = IconKHPivotY_1;

		Rectangle Key_zone = StayOnPivotENDScale_SQRT(Key_side, aa, Key_pivotX, Key_pivotY, 1.0f);
		Rectangle KH_zone = StayOnPivotENDScale_SQRT(KH_side, aa, KH_pivotX, KH_pivotY, 1.0f);

		UI_Obj::DrawTexture_Color(Key_zone, Resources::GetIntroTX_Key(),
			{ Color_ICON_PART.r, Color_ICON_PART.g, Color_ICON_PART.b, Key_Alpha });
		UI_Obj::DrawTexture_Color(KH_zone, Resources::GetIntroTX_Holder(),
			{ Color_ICON_PART.r, Color_ICON_PART.g, Color_ICON_PART.b, KH_Alpha });
	}

	void AnimIcon(float _01, Rectangle aa) {
		float IconWHzone = std::min(aa.width, aa.height);

		float icon_move_01 = CLAMP_01(IconMove_TP0, IconMove_TP1, _01);
		float IPivotX = LERP_01(IconPivotX_0, IconPivotX_1, icon_move_01);
		float IPivotY = LERP_01(IconPivotY_0, IconPivotY_1, icon_move_01);
		float IScale = LERP_01(IconScale_0, IconScale_1, icon_move_01);

		Rectangle IconZone = StayOnPivotENDScale_SQRT(IconWHzone, aa, IPivotX, IPivotY, IScale);

		float bg_progress = CLAMP_01(IconBGUp_TP0, IconBGUp_TP1, _01);
		Rectangle BGZone = StayOnPivotENDScale_SQRT(std::min(IconZone.width, IconZone.height), IconZone, 0.5f, 0.5f,
			LERP_01(BGScale_0, BGScale_1, EaseOutBack(bg_progress, 1.75f)));

		//C(IconZone);
		//C(BGZone);

		unsigned char bgAlpha = 255 * bg_progress;
		UI_Obj::DrawTexture_Color(BGZone, Resources::GetIntroTX_BG(),
			{ Color_ICON_BG .r, Color_ICON_BG.g, Color_ICON_BG.b, bgAlpha });


		float Inside_01 = CLAMP_01(IconInsideAnim_TP0, IconInsideAnim_TP1, _01);

		InsideIconAnim(Inside_01, IconZone);
	}

	void DrawTextCenteredProgress(Font font, const char* text, Vector2 centerPos, float fontSize, Color tint, float progress, float font_div_step) {
		int length = strlen(text);
		if (length == 0) return;

		float autoSpacing = fontSize * font_div_step;

		Vector2 textSize = MeasureTextEx(font, text, fontSize, autoSpacing);

		Vector2 currentGlyphPos = {
			centerPos.x - (textSize.x / 2.0f),
			centerPos.y - (textSize.y / 2.0f)
		};

		const float softness = 2.0f;
		float scaledProgress = progress * (length + softness);

		float scaleFactor = fontSize / (float)font.baseSize;

		for (int i = 0; i < length; i++) {
			// codepoint ( UTF-8, if > 127)
			int codepointSize = 0;
			int codepoint = GetCodepointNext(&text[i], &codepointSize);

			float alpha = (scaledProgress - (float)i) / softness;
			alpha = std::max(0.0f, std::min(1.0f, alpha));

			Color charColor = tint;
			charColor.a = (unsigned char)(tint.a * alpha);

			// Draw Curent
			if (charColor.a > 0) {
				DrawTextCodepoint(font, codepoint, currentGlyphPos, fontSize, charColor);
			}

			int index = GetGlyphIndex(font, codepoint);

			float advance = 0.0f;
			if (font.glyphs[index].advanceX == 0) {
				advance = (float)font.recs[index].width * scaleFactor;
			}
			else {
				advance = (float)font.glyphs[index].advanceX * scaleFactor;
			}

			currentGlyphPos.x += advance + autoSpacing;

			// next
			if (codepointSize > 1) {
				i += (codepointSize - 1);
			}
		}
	}
	void AnimText(float _01, Rectangle aa) {

		float Progtxt_y1_px = ProgNameText_y1 * aa.height;
		float AUtxt_y1_px = AuthorNameText_y1 * aa.height;

		DrawTextCenteredProgress(
			GetFontDefault(),
			Const::PROGRAM_NAME,
			{
				aa.x + aa.width * TextProgNamePivotX,
				aa.y + aa.height * TextProgNamePivotY
			},
			Progtxt_y1_px,
			Const::Colors::TXT_def,
			_01, 0.1f
		);

		static char buffer[256];
		snprintf(buffer, sizeof(buffer), "Code & Design by %s", Const::AUTHOR_NAME);
		DrawTextCenteredProgress(
			Resources::GetFT_main_font_mono(),
			buffer,
			{
				aa.x + aa.width * TextAutNamePivotX,
				aa.y + aa.height * TextAutNamePivotY
			},
			AUtxt_y1_px,
			Const::Colors::TXT_def,
			_01, 0.02f
		);
	}


	float elaps_s = 0.0f;
};

//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void Intro_ST::Draw(Rectangle aa) {

	float window_AR = aa.width / aa.height;
	bool is_w_ref = (drawzone_AR >= window_AR);

	float d_w = aa.width;
	float d_h = d_w / drawzone_AR;

	if (!is_w_ref) {
		d_h = aa.height;
		d_w = d_h * drawzone_AR;
	}

	Rectangle draw_zone
	{
		is_w_ref ? 0.0f : (aa.width - d_w) * 0.5f,
		is_w_ref ? (aa.height - d_h) * 0.5f : 0.0f,
		d_w, d_h
	};

	float _01 = CLAMP_01(AfterStartWait_s, Const::INTRO_DURATION_S + AfterStartWait_s, elaps_s);

	// bg darker
	float bgDark_01 = CLAMP_01(BG_Flade_TP0, BG_Flade_TP1, _01);
	Color RealBGColor = {
		static_cast<unsigned char>(LERP_01(Color_BG_DARK.r, Const::Colors::BG_def.r, bgDark_01)),
		static_cast<unsigned char>(LERP_01(Color_BG_DARK.g, Const::Colors::BG_def.g, bgDark_01)),
		static_cast<unsigned char>(LERP_01(Color_BG_DARK.b, Const::Colors::BG_def.b, bgDark_01)),
		255
	};
	DrawRectangleRec(aa, RealBGColor);

	AnimIcon(_01, draw_zone);
	AnimText(CLAMP_01(TextAnim_TP0, TextAnim_TP1, _01), draw_zone);

	//C(draw_zone);

	elaps_s += TimeStep_s;

	if ((elaps_s - AfterStartWait_s) > Const::INTRO_DURATION_S) {
		Core::END_INTRO();
		elaps_s = 0.0f;
	}
}