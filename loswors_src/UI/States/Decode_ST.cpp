#include "Decode_ST.hpp"

#include "../../Resources/Resources.hpp"
#include "../UI_Obj.hpp"

#include <raygui.h>
#include <string>
#include <cmath>


//      _   _ ___                  _             
//     | | | |_ _|  _ __  __ _ _ _| |___  _ _ __ 
//     | |_| || |  | '  \/ _` | '_| / / || | '_ \
//      \___/|___| |_|_|_\__,_|_| |_\_\\_,_| .__/
// --------------------------------------- |_| -----------

constexpr float
	zone_xdiv = 0.4f,
	tx_inout_xy1_wh = 0.36f,

	linadiv_ypad = 0.096f,

	bigTX_xy1_wh = 0.85f, // by local right zone

	mainzone_outline_wh = 0.005f,

	TXTzone_y0 = 0.21f,
	TXTzone_padx = 0.08f, // by left zone width

	textH_wh = 0.09f,// by local zone to place

	ttt = 0;


namespace {

	// for test only
	void C(Rectangle rt) {
		DrawRectangleLinesEx(rt, 1.0f, RED);
	}

	inline void DrawLeftZone(Rectangle aa, DecodeStatus status, float OutMinS) {
		float sizeTX = std::round(OutMinS * tx_inout_xy1_wh);
		Rectangle inprog_z = { aa.x + (aa.width - sizeTX) * 0.5f, aa.y, sizeTX, sizeTX };
		Rectangle infile_z = { inprog_z.x, aa.y + aa.height - sizeTX, sizeTX, sizeTX };

		float txtpad = std::round(aa.width * TXTzone_padx);
		Rectangle TXT_z = { aa.x + txtpad, aa.y + (OutMinS * TXTzone_y0),
			aa.width - txtpad - txtpad, infile_z.y - (aa.y + (OutMinS * TXTzone_y0)) };
		float textH = std::round(std::min(TXT_z.width, TXT_z.height) * 0.11111f);
		float stepTXT = std::min(textH * 2, (TXT_z.height - textH * 5) * 0.25f);
		
		Color TXovercolor = RED;

		// draw --

		auto DrawTextInPlace = [&](const char* text, bool isActive, float& yPOSpp) {
			Rectangle todraw = { TXT_z.x, TXT_z.y + yPOSpp, TXT_z.width, textH };
			Color fin_C = isActive? BLUE : RED;

			UI_Obj::DrawTextCenter(Resources::GetFT_main_font_mono(), text, todraw, textH, 1.0f, fin_C);
			//DrawTextEx(Resources::GetFT_main_font_mono(), text, { todraw.x, todraw.y }, textH, 1.0f, fin_C); // for left align

			yPOSpp += textH;
			yPOSpp += stepTXT;
		};

		Texture2D inprogtx		= Resources::GetDcdTX_INPROG();
		Texture2D inprogHovertx = Resources::GetDcdTX_INPROG_HOVER();
		Texture2D infiletx		= Resources::GetDcdTX_INFILE();

		DrawTexturePro(inprogHovertx, { 0.0f, 0.0f, (float)inprogHovertx.width, (float)inprogHovertx.height },
			inprog_z, { 0.0f, 0.0f }, 0.0f, Const::Colors::BG_def);
		DrawTexturePro(inprogtx, {0.0f, 0.0f, (float)inprogtx.width, (float)inprogtx.height},
			inprog_z, {0.0f, 0.0f}, 0.0f, TXovercolor);
		
		DrawTexturePro(infiletx, { 0.0f, 0.0f, (float)infiletx.width, (float)infiletx.height },
			infile_z, { 0.0f, 0.0f }, 0.0f, TXovercolor);

		//C(inprog_z);
		//C(infile_z);
		//C(TXT_z);

		float yofPP = (TXT_z.height - (textH * 5 + stepTXT * 4)) * 0.5f;
		DrawTextInPlace("Inside XOR Shuffle",	(status == DecodeStatus::I_XOR_Shuffle), yofPP);
		DrawTextInPlace("Global XOR Shuffle",	(status == DecodeStatus::G_XOR_Shuffle), yofPP);
		DrawTextInPlace("Argon2",				(status == DecodeStatus::Argon2), yofPP);
		DrawTextInPlace("File Hash",			(status == DecodeStatus::FileHash), yofPP);
		DrawTextInPlace("Binary Shuffle",		(status == DecodeStatus::BinShuffle), yofPP);

	}
	inline void DrawRight(Rectangle aa, Texture2D bigTX) {
		float size = std::round(std::min(aa.width, aa.height) * bigTX_xy1_wh);
		Rectangle z = {
			aa.x + std::round((aa.width - size) * 0.5f),
			aa.y + std::round((aa.height - size) * 0.5f),
			size, size
		};

		Color overcolor = GRAY;

		// draw --
		DrawTexturePro(bigTX, { 0.0f, 0.0f, (float)bigTX.width, (float)bigTX.height }, z, { 0.0f, 0.0f }, 0.0f, overcolor);
		//DrawRectangleLinesEx(z, 1.0f, BLUE);
	}

};


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void Decode_ST::Draw(Rectangle aa, DecodeStatus status) {

	float minS = std::min(aa.width, aa.height);

	Rectangle lz = { aa.x, aa.y, std::round(aa.width * zone_xdiv), aa.height };
	Rectangle rz = { lz.x + lz.width, aa.y, aa.width - lz.width, aa.height };

	float divLpad = aa.height * linadiv_ypad;
	float line_W = std::max(1.0f, std::ceil(minS * mainzone_outline_wh));

	Texture2D bigTX;
	switch (status)
	{
		case DecodeStatus::BinShuffle:
			bigTX = Resources::GetDcdTX_BIN_Shuffle();		break;
		case DecodeStatus::FileHash:
			bigTX = Resources::GetDcdTX_FileHash();			break;
		case DecodeStatus::Argon2:
			bigTX = Resources::GetDcdTX_Argon2();			break;
		case DecodeStatus::G_XOR_Shuffle:
			bigTX = Resources::GetDcdTX_G_XOR_Shuffle();	break;
		case DecodeStatus::I_XOR_Shuffle:
			bigTX = Resources::GetDcdTX_I_XOR_Shuffle();	break;
		default:
			bigTX = Resources::GetTX_cros();
	}

	Color lineOvercolor = RED;

	// draw // -- // -- //

	DrawRectangleLinesEx(aa, line_W, lineOvercolor);
	DrawLineEx({ rz.x, rz.y + divLpad }, { rz.x, rz.y + rz.height - divLpad }, line_W, lineOvercolor);

	//C(lz); C(rz);

	DrawLeftZone(lz, status, minS);
	DrawRight(rz, bigTX);
}