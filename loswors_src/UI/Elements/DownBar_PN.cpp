#include "DownBar_PN.hpp"
#include <raygui.h>

#include <cmath>

#include "../../CORE/Log.hpp"

#include "../../Localization/LocalSys.hpp"
#include "../../Localization/loc.hpp"

#include "../UI.hpp"
#include "../UI_Obj.hpp"
#include "../NotifService.hpp"

#include "../Theme/Theme.hpp"
#include "../../Resources/Resources.hpp"

#define IS_RT_CHECK 0

#if IS_RT_CHECK
static inline void C(Rectangle rt) {
	DrawRectangleLinesEx(rt, 1, RED);
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
	log_spañe = 0.80f,	//  %  0.8 -> |01234567|89|
	act_log_zone_factor = 0.6f,
	act_log_inside_pad_wh = 0.04f,
	textH_wh = 0.03f,

	version_header_text_y_wh = 0.47f, // for all text & btn in start content

	version_textH_wh = 0.04f;

namespace {

	bool ppLOG = false;
	bool ppVER = false;
	bool ppESC = false;

	float TXTH = 15.0f; // reqalc !
	inline void updateTXTH() {
		TXTH = std::round(UI::GetMinWH() * textH_wh);
	}

	inline void versionbtn(Rectangle aa) {
		Theme::SS_db_version(TXTH);

		Rectangle btnz = {
			std::round(aa.x + (aa.width * log_spañe)),
			aa.y,
			std::round(aa.width * (1.0f - log_spañe)),
			aa.height
		};

		bool isok = UI_Obj::isPressInZ(btnz, ppVER);
		if (GuiButton(btnz, Const::LOSWORS_VERSION_TXT) && isok) {
			UI::Set_FSP(FS_Panel::VERSION);
		}
	}
	inline void logbtn(Rectangle aa) {
		Theme::SS_db_log(Log::GetLast().getColor(), TXTH);

		Rectangle btnz = {
			aa.x,
			aa.y,
			std::round(aa.width * log_spañe),
			aa.height
		};

		bool isok = UI_Obj::isPressInZ(btnz, ppLOG);
		if (GuiButton( btnz, Log::GetLast().getStr()) && isok) {
			UI::Set_FSP(FS_Panel::LOG);
		}
	}

	Vector2 v_scroll = { 0,0 };

	Vector2 log_scroll_offset = { 0,0 };
	float log_prev_content_h = 0;
	Rectangle log_view_zone = { 0 };

	int ACT_LOG_ID = -1; // -1 -> none selected

	// Log helper --------------------------------------------
	inline bool DrawOneLog(Rectangle rect, one_log log, Color bg_color)
	{
		DrawRectangleRec(rect, bg_color); // bg

		int padding = 4;
		Font font = Resources::GetFT_main_font();

		DrawTextEx(
			font,
			log.getStr(),
			{ rect.x + padding, rect.y + padding },
			TXTH,
			1,
			log.getColor()
		);
		if (ACT_LOG_ID != -1) return false;
		return (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
			CheckCollisionPointRec(GetMousePosition(), rect) &&
			MeasureTextEx(font, log.getStr(), TXTH, 1).x > rect.width);
	}
	inline void Draw_arr_log(Rectangle area, float row_height) {
		Theme::SS_db_log_scroll();
		float content_h = Log::GetSize() * row_height;

		// Add a small margin (offset) so that the text does not stick to the scrollbar
		int slider_w = (content_h > area.height) ? GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) : 0;
		Rectangle content_size = { 0, 0, 100, content_h };

		// if new log content_h expand slider as down orient
		if (content_h > log_prev_content_h) {
			log_scroll_offset.y -= (content_h - log_prev_content_h);
		}
		log_prev_content_h = content_h;
		GuiScrollPanel(area, NULL, content_size, &log_scroll_offset, &log_view_zone);
		BeginScissorMode((int)area.x, (int)area.y, (int)area.width - slider_w, (int)area.height);

		// if log_h < (aval. spase h) do nosing with slider pading
		float baseY = (content_h < area.height) ? (area.height - content_h) : 0;

		// optimize: draw only log IN screen space
		int first_id = std::max(0, (int)(-log_scroll_offset.y / row_height));

		Color def = Theme::C::logs_bg_def;
		Color dark_def = Theme::C::logs_bg_dark;

		for (int i = first_id; i < Log::GetSize(); i++) {

			float y_pos = area.y + log_scroll_offset.y + (i * row_height) + baseY;

			// limit draw
			if (y_pos + row_height < area.y) continue;		// upper
			if (y_pos > area.y + area.height) break;		// downper

			Rectangle itemRect = {
				area.x,
				y_pos,
				area.width - slider_w,
				row_height
			};

			if (DrawOneLog(itemRect, Log::GetById(i), (i % 2 == 0) ? def : dark_def)) {
				// save the ID & show log in full 
				ACT_LOG_ID = i;
			}
		}

		EndScissorMode();
	}
	inline void DrawACTlog(Rectangle aa) {
		if (ACT_LOG_ID < 0 || ACT_LOG_ID >= Log::GetSize()) return;

		Rectangle z = { aa.x, aa.y,
			std::round(aa.width * act_log_zone_factor), std::round(aa.height * act_log_zone_factor) };
		z.x += std::round((aa.width - z.width) * 0.5f);
		z.y += std::round((aa.height - z.height) * 0.5f);

		constexpr unsigned char dark = 128;

		DrawRectangle((int)aa.x, (int)aa.y, (int)aa.width, (int)aa.height, { 0,0,0,dark });
		DrawRectangle((int)z.x, (int)z.y, (int)z.width, (int)z.height, Theme::C::log_fullscreen_bg);
		DrawRectangleLinesEx(z, 1.0f, Theme::C::log_fullscreen_line);

		auto drawLOG = [&]() {
			one_log l = Log::GetById(ACT_LOG_ID);
			Font font = Resources::GetFT_main_font_mono();
			const char* text = l.getStr();

			constexpr int ypadPX = 3;

			int totalChars = GetCodepointCount(text);
			if (totalChars == 0) return;

			Vector2 leterSZ = MeasureTextEx(font, "A", TXTH, 1.0f);

			int maxSIMBOLcount = (int)(z.width / leterSZ.x);
			if (maxSIMBOLcount <= 0) maxSIMBOLcount = 1;

			int lineCount = (totalChars + maxSIMBOLcount - 1) / maxSIMBOLcount;

			float textH = lineCount * leterSZ.y + (lineCount - 1) * ypadPX;
			float startY = z.y + (z.height - textH) * 0.5f;

			// --- UTF-8 ---
			const char* ptr = text;
			int bytesProcessed = 0;

			for (int charIdx = 0; charIdx < totalChars; charIdx++) {
				int codepointByteSize = 0;
				int codepoint = GetCodepointNext(ptr, &codepointByteSize);
				ptr += codepointByteSize;

				int line = charIdx / maxSIMBOLcount;
				int col = charIdx % maxSIMBOLcount;

				int charsInThisLine = (line == lineCount - 1) ? (totalChars - line * maxSIMBOLcount) : maxSIMBOLcount;

				float lineWidth = charsInThisLine * leterSZ.x;
				float startX = z.x + (z.width - lineWidth) * 0.5f;

				Vector2 pos = {
					startX + col * leterSZ.x,
					startY + line * (leterSZ.y + ypadPX)
				};

				DrawTextCodepoint(font, codepoint, pos, TXTH, l.getColor());
			}
		};

		float padT = std::round(std::min(z.width, z.height) * act_log_inside_pad_wh);
		z.x += padT;
		z.y += padT;
		z.width -= (padT + padT);
		z.height -= (padT + padT);

		drawLOG();

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			ACT_LOG_ID = -1;
	}

	inline void DrawBTNExit(Rectangle zone) {
		Theme::SS_db_exit(TXTH);

		bool isok = UI_Obj::isPressInZ(zone, ppESC);

		if ((GuiButton(zone, loc::t.downbar.closeesc) && isok) || IsKeyPressed(KEY_ESCAPE)) {
			v_scroll = { 0,0 };
			UI::Set_FSP(FS_Panel::NONE);
		}
	}
}


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void DownBar_PN::DrawDownBar(Rectangle aa) {
	ACT_LOG_ID = -1;
	updateTXTH();
	// version
	versionbtn(aa);
	// log
	logbtn(aa);
}

void DownBar_PN::DrawLog(struct Rectangle aa_zone, struct Rectangle aa_db) {
	updateTXTH();

	UI::Set_HeaderTextStatus(HeaderTextState::LOG);

	if (IsKeyPressed(KEY_ESCAPE)) {
		UI::Set_FSP(FS_Panel::NONE);
	}

	// LOG`S _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

	int tempID = ACT_LOG_ID;
	Draw_arr_log(aa_zone, aa_db.height);
	if (tempID == ACT_LOG_ID) DrawACTlog(aa_zone);
	

	// BAR _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

	DrawBTNExit(
		{
			aa_db.x,
			aa_db.y,
			std::round(aa_db.width * log_spañe),
			aa_db.height
		});

	// version
	versionbtn(aa_db);
}

// for version
namespace {
	Vector2 version_prev_size = { 0,0 };
	int version_prev_lang_id = -1;

	const Color color_bgSubHeader  = Color{ 100, 0, 0, 255 };
	const Color color_bgHeader = Color{ 60, 0, 0, 255 };

	constexpr float pad_factor = 0.3f, header_factor = 1.6f;
	struct VersionInfoZone {

		bool isGOTO = false;

		int status = 0; // 0 normal header text, 1 subtopic header text
		const char* headerTXT = nullptr;
		const char* infoTXT = nullptr;

		int tx_align = -1; // 0 left 1 right
		Texture2D img_tx = Texture2D{};
		float tx_factor = 0.1f;

		float txtH = 0;
		
		std::vector<Rectangle> preqalc_txts_simbol_zone;
		Rectangle preqalc_tx_rt = {};
		Vector2 preqalc_size = { -1,-1 }; // preqalc size

		VersionInfoZone() = default;

		VersionInfoZone(int status_, const char* headerTXT_, const char* infoTXT_, int tx_align_, Texture2D img_tx_, float tx_factor_)
			: status(status_), headerTXT(headerTXT_), infoTXT(infoTXT_), tx_align(tx_align_), img_tx(img_tx_), tx_factor(tx_factor_) {}

		void ReQalc(Rectangle g_aa, float txtH_) {
			isGOTO = false;
			txtH = txtH_;
			preqalc_txts_simbol_zone.clear();

			Font font = Resources::GetFT_main_font();
			const float letterSpacing = 1.0f;

			float padding = txtH * pad_factor;
			float headerHeight = txtH * header_factor;
			float availableWidth = g_aa.width - (padding * 2.0f);

			// 1. Ðàñ÷åò ðàçìåðà êàðòèíêè
			float txWidth = 0.0f;
			float txHeight = 0.0f;
			bool hasImage = (img_tx.id > 0);

			if (hasImage) {
				float minSide = std::min(g_aa.width, g_aa.height);
				float baseSize = minSide * tx_factor;
				float aspect = (img_tx.width > 0) ? (float)img_tx.height / (float)img_tx.width : 1.0f;

				txWidth = baseSize;
				txHeight = baseSize * aspect;
			}

			float textBlockWidth = availableWidth;
			if (hasImage) {
				textBlockWidth -= (txWidth + padding * 2);
			}
			if (textBlockWidth < 50.0f) textBlockWidth = availableWidth;

			float currentTextHeight = 0.0f;
			float lineHeight = txtH * 1.1f;

			if (infoTXT && strlen(infoTXT) > 0) {
				std::string textStr(infoTXT);

				struct CharUnit {
					int cp;
					std::string utf8Str;
					float baseWidth; // Pure width of a glyph without spacing
					bool isSpace;
					bool isNewline;
				};

				std::vector<CharUnit> allUnits;
				size_t i = 0;
				size_t len = textStr.length();

				while (i < len) {
					int bytesRead = 0;
					int cp = GetCodepointNext(&textStr[i], &bytesRead);
					if (cp == 0) break;

					char temp[5] = { 0 };
					if (bytesRead > 0 && bytesRead < 5) {
						memcpy(temp, &textStr[i], bytesRead);
					}

					bool isSpace = (cp == ' ' || cp == '\t');
					bool isNewline = (cp == '\n' || cp == '\r');

					float w = 0.0f;
					if (!isNewline) {
						if (isSpace) {
							w = txtH * 0.25f; // Fixed fixed width space
						}
						else {
							// Measuring the pure width of the character with spacing = 0.0f (without internal engine bugs)
							w = MeasureTextEx(font, temp, txtH, 0.0f).x;
						}
					}

					allUnits.push_back({ cp, std::string(temp), w, isSpace, isNewline });
					i += bytesRead;
				}

				std::vector<Rectangle> localSymbols(allUnits.size(), { 0, 0, 0, 0 });

				float currentX = 0.0f;
				float currentY = 0.0f;

				size_t idx = 0;
				while (idx < allUnits.size()) {
					if (allUnits[idx].isNewline) {
						localSymbols[idx] = { currentX, currentY, 0.0f, txtH };
						currentX = 0.0f;
						currentY += lineHeight;
						idx++;
						continue;
					}

					if (allUnits[idx].isSpace) {
						if (currentX == 0.0f) {
							localSymbols[idx] = { currentX, currentY, 0.0f, txtH };
						}
						else {
							localSymbols[idx] = { currentX, currentY, allUnits[idx].baseWidth, txtH };
							currentX += allUnits[idx].baseWidth + letterSpacing;
						}
						idx++;
						continue;
					}

					// Collecting a whole word
					size_t wordStart = idx;
					float wordWidth = 0.0f;
					size_t tempIdx = idx;

					while (tempIdx < allUnits.size() && !allUnits[tempIdx].isSpace && !allUnits[tempIdx].isNewline) {
						wordWidth += allUnits[tempIdx].baseWidth + letterSpacing;
						tempIdx++;
					}
					// Remove the extra interval after the last letter of the word
					if (tempIdx > wordStart) {
						wordWidth -= letterSpacing;
					}

					// Move the word to a new line if it doesn't fit
					if (currentX > 0.0f && currentX + wordWidth > textBlockWidth) {
						currentX = 0.0f;
						currentY += lineHeight;
					}

					// Linear arrangement of letters in a word with addition of letterSpacing
					float charX = currentX;
					for (size_t k = wordStart; k < tempIdx; ++k) {
						localSymbols[k] = {
							std::round(charX),
							std::round(currentY),
							allUnits[k].baseWidth,
							txtH
						};
						charX += allUnits[k].baseWidth + letterSpacing;
					}

					currentX = charX; // Move to the next position considering the indentation after the word
					idx = tempIdx;
				}

				if (!localSymbols.empty()) {
					currentTextHeight = currentY + lineHeight;
				}

				preqalc_txts_simbol_zone = std::move(localSymbols);
			}

			// 3. Calculating the total content height and vertical offsets (centering)
			float contentHeight = std::max(currentTextHeight, txHeight);
			float textOffsetY = 0.0f;
			float txOffsetY = 0.0f;

			if (hasImage) {
				if (currentTextHeight < contentHeight) {
					textOffsetY = (contentHeight - currentTextHeight) * 0.5f;
				}
				if (txHeight < contentHeight) {
					txOffsetY = (contentHeight - txHeight) * 0.5f;
				}
			}

			// 4. Horizontal positioning
			float textStartX = padding;
			float txStartX = padding;

			if (hasImage) {
				if (tx_align == 1) {
					txStartX = g_aa.width - padding - txWidth;
					textStartX = padding;
				}
				else {
					tx_align = 0;
					txStartX = padding;
					textStartX = padding + txWidth + padding;
				}
			}

			float baseContentY = headerHeight + padding;

			for (auto& sym : preqalc_txts_simbol_zone) {
				sym.x += textStartX;
				sym.y += baseContentY + textOffsetY;
			}

			if (hasImage) {
				preqalc_tx_rt = { txStartX, baseContentY + txOffsetY, txWidth, txHeight };
			}
			else {
				preqalc_tx_rt = { 0, 0, 0, 0 };
			}

			preqalc_size.x = g_aa.width;
			preqalc_size.y = baseContentY + contentHeight + padding;
		}

		void Draw(float& yof) {
			float startY = yof;
			float padding = txtH * pad_factor;
			float headerHeight = txtH * header_factor;

			Color bgColor = (status == 0) ? color_bgHeader : color_bgSubHeader;
			Color borderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
			Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));

			// --- Draw Header ---
			Rectangle headerRect = { 0, startY, preqalc_size.x, headerHeight };
			DrawRectangleRec(headerRect, bgColor);
			//DrawRectangleLinesEx(headerRect, 1.0f, borderColor);

			Font font = GuiGetFont();
			if (headerTXT) {
				if (status == 0) {
					float textW = MeasureTextEx(font, headerTXT, txtH, 1.0f).x;
					Vector2 textPos = { headerRect.x + (headerRect.width - textW) * 0.5f, headerRect.y + (headerHeight - txtH) * 0.5f };
					DrawTextEx(font, headerTXT, textPos, txtH, 1.0f, textColor);
				}
				else {
					std::string subHeader = "<< " + std::string(headerTXT);
					Vector2 textPos = { headerRect.x + padding, headerRect.y + (headerHeight - txtH) * 0.5f };
					DrawTextEx(font, subHeader.c_str(), textPos, txtH, 1.0f, textColor);
				}
			}

			// --- Drawing the Image ---
			if (img_tx.id > 0) {
				Rectangle dest = preqalc_tx_rt;
				dest.y += startY;
				DrawTexturePro(img_tx, { 0, 0, (float)img_tx.width, (float)img_tx.height }, dest, { 0, 0 }, 0.0f, textColor);
			}

			// --- Drawing Text 1-by-1 by indices ---
			if (infoTXT && strlen(infoTXT) > 0) {
				std::string textStr(infoTXT);
				size_t i = 0;
				size_t index = 0;

				while (i < textStr.length() && index < preqalc_txts_simbol_zone.size()) {
					int bytesRead = 0;
					int cp = GetCodepointNext(&textStr[i], &bytesRead);
					if (cp == 0) break;

					// If this is a regular symbol (not a space or newline) — render it
					if (cp != '\n' && cp != '\r' && cp != ' ' && cp != '\t') {
						char temp[5] = { 0 };
						if (bytesRead > 0 && bytesRead < 5) {
							memcpy(temp, &textStr[i], bytesRead);
						}

						Rectangle posRect = preqalc_txts_simbol_zone[index];
						Vector2 drawPos = { posRect.x, posRect.y + startY };

						DrawTextEx(font, temp, drawPos, txtH, 1.0f, textColor);
					}

					i += bytesRead;
					index++;
				}
			}

			yof += preqalc_size.y;
		}
	};

	constexpr size_t varr_count = 9;
	VersionInfoZone varr[varr_count] = {};

	float v_content_h = 0;

	inline void draw_V_content(Rectangle aa) {
		Theme::SS_db_version_scroll();

		int slrdW = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

		const float minS = std::min(aa.width, aa.height);
		const float xof = std::round(minS * 0.05f);
		const float xoftab = std::round(minS * 0.2f);

		float goto_btnTXTh = std::round(minS * 0.05f);
		float goto_btnpad = std::round(goto_btnTXTh * 0.7f);
		float exp_goto_btn_h = std::round(goto_btnTXTh * 1.0f);
		float goto_line_pad = std::round(exp_goto_btn_h * 0.5f);

		float header_h = std::round(minS * version_header_text_y_wh);


		auto reQ = [&]() {

			// Set data ---------------------------------------

			varr[0] = VersionInfoZone(0, loc::t.version.h_privacy, loc::t.version.c_privacy, -1, Texture2D(), 0.0f);
			varr[1] = VersionInfoZone(1, loc::t.version.h_license, loc::t.version.c_license, -1, Texture2D(), 0.0f);

			varr[2] = VersionInfoZone(0, loc::t.version.h_motiv, loc::t.version.c_motiv, -1, Texture2D(), 0.0f);

			varr[3] = VersionInfoZone(0, loc::t.version.h_howitwork, loc::t.version.c_howitwork, -1, Texture2D(), 0.0f);
			varr[4] = VersionInfoZone(1, "Binary Shuffle", loc::t.version.c_binshuffle, 1,
				Resources::GetDcdTX_BIN_Shuffle(), 0.5f);
			varr[5] = VersionInfoZone(1, "File Hash", loc::t.version.c_filehash, 0,
				Resources::GetDcdTX_FileHash(), 0.5f);
			varr[6] = VersionInfoZone(1, "Argon2id", loc::t.version.c_argon2id, 1,
				Resources::GetDcdTX_Argon2(), 0.5f);
			varr[7] = VersionInfoZone(1, "Global XOR Shuffle", loc::t.version.c_gxor, 0,
				Resources::GetDcdTX_G_XOR_Shuffle(), 0.5f);
			varr[8] = VersionInfoZone(1, "Inside XOR Shuffle", loc::t.version.c_ixor, 1,
				Resources::GetDcdTX_I_XOR_Shuffle(), 0.5f);

			// reqalc -----------------------------------------
			v_content_h = 0;
			v_content_h += header_h;
			v_content_h += goto_btnpad;
			
			float txtH = std::round(std::min(aa.width, aa.height) * version_textH_wh);
			for (size_t i = 0; i < varr_count; ++i) {
				varr[i].ReQalc({aa.x, aa.y, aa.width - slrdW, aa.height}, txtH);	
				v_content_h += varr[i].preqalc_size.y;

				// addit
				v_content_h += exp_goto_btn_h;
				v_content_h += goto_btnpad;
			}
			v_content_h = std::round(v_content_h);
		};
		if ((version_prev_size.x != aa.width || version_prev_size.y != aa.height) ||
			LocalSys::GetCurLangID() != version_prev_lang_id)
		{
			reQ();

			version_prev_size.x = aa.width;
			version_prev_size.y = aa.height;

			version_prev_lang_id = LocalSys::GetCurLangID();
		}

		Rectangle view = {};
		GuiScrollPanel(aa, nullptr, { 0, 0, aa.width - slrdW, v_content_h }, &v_scroll, &view);

		BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

		Font font = Resources::GetFT_main_font();

		float yof = aa.y + v_scroll.y;

		// |  header
		// |___________________

		Rectangle header_z = { aa.x, aa.y, aa.width - slrdW, header_h };
		constexpr float name_y1 = 0.38f, fullname_y1 = 0.1f, ver_pad_y1 = 0.11f, ver_cred_y1 = 0.08f;
		constexpr float btns_zone_y1 = 1.0f - (name_y1 + fullname_y1 + ver_pad_y1 + ver_cred_y1 + ver_cred_y1);

		auto getRT_header = [&](const float coef, float& yofpp) -> Rectangle {
			Rectangle ret = {
				header_z.x, yofpp,
				header_z.width, std::round(header_z.height * coef)
			};
			yofpp += ret.height;
			return ret;
		};

		Rectangle name_z		= getRT_header(name_y1, yof);
		Rectangle fullname_z	= getRT_header(fullname_y1, yof); fullname_z.y -= fullname_z.height * 0.5f;
		yof += std::round(header_z.height * ver_pad_y1);
		Rectangle ver_z			= getRT_header(ver_cred_y1, yof);
		Rectangle cred_z		= getRT_header(ver_cred_y1, yof);
		Rectangle btns_z		= getRT_header(btns_zone_y1, yof);
		// |--[btn]-[btn]--|
		float btn_y1 = std::round(btns_z.height * 0.55f);
		float btn_x1 = std::round(btns_z.width * 0.27f);
		float space_x_PX = std::round((btns_z.width - btn_x1 - btn_x1) * 0.2f);
		Rectangle gitbtn_z = {
			btns_z.x + space_x_PX + space_x_PX,
			std::round(btns_z.y + (btns_z.height - btn_y1) * 0.38f), // not 0.5f for divider vizible normal
			btn_x1, btn_y1 };
		Rectangle introbtn_z = {
			gitbtn_z.x + gitbtn_z.width + space_x_PX, gitbtn_z.y,
			btn_x1, btn_y1 };

		// if & draw ---
		C(name_z);
		C(fullname_z);
		C(ver_z);
		C(cred_z);
		C(btns_z);

		auto is_in = [](const Rectangle& aa, const Rectangle& content) -> bool {
			return (content.y + content.height > aa.y);
		};

		Color color_TOK = WHITE;
		Color color_TVC = { 255, 255, 255, 128 };

		if (is_in(aa, name_z))
			UI_Obj::DrawText_FitCenter(name_z, Const::PROGRAM_NAME, Resources::GetFT_main_BIGNAME_font(), color_TOK);
		if (is_in(aa, fullname_z))
			UI_Obj::DrawText_FitCenter(fullname_z, "Local Password Saver", font, color_TOK);
		if (is_in(aa, ver_z))
			UI_Obj::DrawText_FitCenter(ver_z, Const::LOSWORS_VERSION_TXT, font, color_TVC);
		if (is_in(aa, cred_z))
			UI_Obj::DrawText_FitCenter(cred_z, "Code & Design by HARD_VVV", font, color_TVC);

		Theme::SS_db_version_scroll_btns(); // theme normalize --

		GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLUE));
		if (UI_Obj::BTN_Text_FitCenter(gitbtn_z, "GitHub", font, 0.2f)) {
			OpenURL(Const::PROGRAM_GITURL);
			//SetClipboardText(Const::PROGRAM_GITURL);
			//NotifService::AddMessage(MType::M_NORMAL, loc::t.notif.copydone0, loc::t.notif.copydone1);
		}

		GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(RED));
		if (UI_Obj::BTN_Text_FitCenter(introbtn_z, "Intro", font, 0.2f)) {
			Core::PLAY_INTRO_FROM_VERSION();
		}

		Theme::SS_db_version_scroll_btns(); // theme normalize --

		DrawLineEx({ btns_z.x, btns_z.y + btns_z.height}, {btns_z.x + btns_z.width, btns_z.y + btns_z.height}, 
			3.0f, BLACK);

		float start_blackout = (btns_z.y + btns_z.height < aa.y) ? aa.y : btns_z.y + btns_z.height;
		DrawRectangleRec( { btns_z.x, start_blackout, btns_z.width, (aa.y + aa.height) - start_blackout }, {0,0,0,40});


		// |  Go To
		// |___________________

#if IS_RT_CHECK
		Rectangle goto_rt_check = { aa.x, yof, aa.width - slrdW, 0 };
#endif

		yof += goto_btnpad;

		for (size_t i = 0; i < varr_count; ++i) {

			if (yof + (exp_goto_btn_h) < aa.y) {
				yof += exp_goto_btn_h;
				yof += goto_btnpad;
				continue;
			}

			bool isSUB = (varr[i].status == 1);

			Color lineC = GRAY;
			Color bgC = (isSUB) ? color_bgSubHeader : color_bgHeader;

			GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(bgC));
			GuiSetStyle(BUTTON, BORDER_WIDTH, 0);

			Rectangle bz;
			float c_xof = (isSUB) ? xoftab : xof;
			float center_y = std::round(yof + exp_goto_btn_h * 0.5f);

			if (UI_Obj::BTN_Expand_By_Text({ c_xof, center_y },
				{ 0.0f,0.5f }, varr[i].headerTXT, font, goto_btnTXTh, 1, &bz)) {
				varr[i].isGOTO = true;
			}
			
			DrawLineEx({ bz.x + bz.width + goto_line_pad, center_y }, { view.x + view.width - xof,center_y }, 2.0f, lineC);
			if (isSUB) {
				DrawLineEx({ view.x + xof, center_y }, { bz.x - goto_line_pad, center_y }, 2.0f, lineC);
			}

			yof += exp_goto_btn_h;
			yof += goto_btnpad;

			if (yof > aa.y + aa.height) break; // not return (GOTO logic need to work)!
		}

#if IS_RT_CHECK
		goto_rt_check.height = yof - goto_rt_check.y;
		C(goto_rt_check);
#endif

		// |  Main data Data
		// |___________________
		float goto_stepPX = std::ceil(v_content_h * 0.02f);

		for (size_t i = 0; i < varr_count; ++i) {
			if (varr[i].isGOTO) {
				if (yof < aa.y) {
					v_scroll.y += goto_stepPX;
				}
				else if (yof > aa.y) {
					v_scroll.y -= goto_stepPX;
				}

				if (yof - aa.y <= goto_stepPX) {
					v_scroll.y += (goto_stepPX - (yof - aa.y)); v_scroll.y = std::round(v_scroll.y);
					varr[i].isGOTO = false;
				}
				else if (v_scroll.y + v_content_h < aa.height) {
					varr[i].isGOTO = false;
				}

			}

			if (yof + varr[i].preqalc_size.y < aa.y ||
				yof > aa.y + aa.height)
			{
				yof += varr[i].preqalc_size.y;
				// NO need early returm (GOTO logic need to work)!
				continue;
			}

			varr[i].Draw(yof);
		}
	}
}

void DownBar_PN::DrawVersion(struct Rectangle aa_zone, struct Rectangle aa_db) {
	ACT_LOG_ID = -1;
	updateTXTH();
	UI::Set_HeaderTextStatus(HeaderTextState::VERSION);

	draw_V_content(aa_zone); EndScissorMode();

	logbtn(aa_db);

	DrawBTNExit(
	{
		std::round(aa_db.x + (aa_db.width * log_spañe)),
		aa_db.y,
		std::round(aa_db.width * (1.0f - log_spañe)),
		aa_db.height
	});
}

void DownBar_PN::Reset() {
	v_scroll = { 0.0f, 0.0f };
}


