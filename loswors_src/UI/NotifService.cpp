#include "NotifService.hpp"

#include "../CORE/Const.hpp"
#include "UI.hpp"

//      _   _ ___                        _                
//     | | | |_ _|  _ __ ___   __ _ _ __| | ___   _ _ __  
//     | | | || |  | '_ ` _ \ / _` | '__| |/ / | | | '_ \ 
//     | |_| || |  | | | | | | (_| | |  |   <| |_| | |_) |
//      \___/|___| |_| |_| |_|\__,_|_|  |_|\_\\__,_| .__/ 
// ----------------------------------------------- |_| ---

constexpr Color
	c_normal = { 128, 158, 191, 255 },
	c_warning = { 191, 186, 128, 255 },
	c_error = { 191, 128, 128, 255 };

constexpr float
	pad_wh = 0.08f,
	line_h_y0 = 0.35f, // 35
	esc_btn_xy1_wh = 0.8f,
	zone_x1 = 0.6f,
	zone_y1 = 0.13f,
	DT = 1.0f / Const::TARGET_FPS,
	time_w_dure = 4.5f,		// time (s) wait
	time_anim = 0.2f;		// time (s) anim

#define RT_TEST 0 

#if RT_TEST
inline static void C(Rectangle rt) {
	DrawRectangleLinesEx(rt, 1.0f, RED);
}
#else
inline static void C(Rectangle) {}
#endif

namespace {

	bool isHoverCursor = false;

	struct Message {
		MType type;
		std::string name;		// small upper large
		std::string text;		// message
		unsigned int tic = 0;	// (raygui tic) after add
	};


	std::vector<Message> messages;  // `s

	// for name
	inline void DrawName(const char* text, Rectangle bounds, Color color) {
		if (text == nullptr || text[0] == '\0') return;

		Font font = Resources::GetFT_main_font();
		float fontSize = bounds.height;

		// 1. Measure text size at the default font size
		Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.0f);

		// 2. If it doesn't fit, decrease the font size
		if (textSize.x > bounds.width) {
			float scale = bounds.width / textSize.x;
			fontSize *= scale;
			//textSize = MeasureTextEx(font, text, fontSize, 1.0f);
		}

		Vector2 position = {
			bounds.x,
			bounds.y + (bounds.height - textSize.y) * 0.5f
		};

		BeginScissorMode((int)bounds.x, (int)bounds.y, (int)bounds.width, (int)bounds.height);
		// Spacing kept as standard 1.0f
		DrawTextEx(font, text, position, fontSize, 1.0f, color);
		EndScissorMode();
	}
	// for desc
	inline static void DrawDecs(const char* text, Rectangle bounds, Color color) {
		float minFontSize = 5.0f;
		float low = minFontSize;
		float high = UI::GetTextSize();
		float bestFontSize = minFontSize;
		Vector2 bestTextSize = { 0, 0 };

		// In total, 6 binary search iterations will find the size with a precision of ~0.5px
		for (int i = 0; i < 6; ++i) {
			float mid = (low + high) * 0.5f;
			Vector2 size = MeasureTextEx(Resources::GetFT_main_font(), text, mid, 1.0f);

			if (size.x <= bounds.width && size.y <= bounds.height) {
				bestFontSize = mid;
				bestTextSize = size;
				low = mid; // Trying to make it more
			}
			else {
				high = mid; // Not enough space, try less
			}
		}

		Vector2 position = {
			bounds.x,
			bounds.y + (bounds.height - bestTextSize.y) * 0.5f
		};

		DrawTextEx(Resources::GetFT_main_font(), text, position, bestFontSize, 1.0f, color);
	}
	


	inline bool Draw_m(size_t i, Rectangle r_baze) {

		float time = DT * messages[i].tic;

		float wait_persent = 0;

		if (time < time_anim) {
			// anim forvard 
			r_baze.y += -r_baze.height + r_baze.height * (time / time_anim);
		}
		else if (time > time_w_dure + time_anim) {
			// anim back
			wait_persent = 1;
			r_baze.y += -r_baze.height * ((time - (time_w_dure + time_anim)) / time_anim);
		}
		else {
			// anim stop
			wait_persent = (time - time_anim) / time_w_dure;
			r_baze.y += 0;
		}
		if (time > time_w_dure + time_anim * 2) {
			return true;
		}

		// draw -------------------------

		Color c_to_use = c_normal;

		if (messages[i].type == MType::M_NORMAL) {
			c_to_use = c_normal;
		}
		else if (messages[i].type == MType::M_WARNING) {
			c_to_use = c_warning;
		}
		else if (messages[i].type == MType::M_ERROR) {
			c_to_use = c_error;
		}

		float min_wh = std::min(r_baze.width, r_baze.height);
		float padding = min_wh * pad_wh;

		float header_height = r_baze.height * line_h_y0;

		Color c_txt = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
		c_txt = BLACK;

		// BG
		DrawRectangleRec(r_baze, c_to_use);
		DrawRectangleLinesEx(r_baze, 2.0f, c_txt);

		// Name
		Rectangle titlePos = {
			r_baze.x + padding * 2,
			r_baze.y + padding * 0.5f,
			r_baze.width - (esc_btn_xy1_wh * min_wh) - padding * 4,
			header_height
		};
		C(titlePos);

		DrawName(messages[i].name.c_str(), titlePos, c_txt);

		// Close Btn
		Rectangle closeRect = {
			r_baze.x + (r_baze.width - (esc_btn_xy1_wh * min_wh + padding)),
			r_baze.y + r_baze.height * 0.5f - (esc_btn_xy1_wh * min_wh) * 0.5f,
			esc_btn_xy1_wh * min_wh,
			esc_btn_xy1_wh * min_wh
		};

		// Line devider
		DrawLineEx(
			{
				r_baze.x + padding,
				r_baze.y + header_height + padding
			},
			{ r_baze.x + padding + (r_baze.width - closeRect.width - padding * 3) * wait_persent, r_baze.y + header_height + padding }, 2.0f, c_txt
		);
		if (UI_Obj::BTN_Image(closeRect, Resources::GetTX_cros(), RED, 0)) {
			return true;
		}

		// text main
		Rectangle msgPos = {
			r_baze.x + padding * 2,
			r_baze.y + header_height + padding * 2,
			r_baze.width - (esc_btn_xy1_wh * min_wh) - padding * 4,
			r_baze.height - titlePos.height - padding * 4
		};
		C(msgPos);
		DrawDecs(messages[i].text.c_str(), msgPos, c_txt);

		return false;
	}

}


//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

void NotifService::Draw(Rectangle aa) {

	size_t len = messages.size();

	Rectangle r_baze{
		aa.x + (aa.width - (aa.width * zone_x1)) * 0.5f,
		aa.y,
		aa.width * zone_x1,
		aa.height * zone_y1
	};
	isHoverCursor = CheckCollisionPointRec(GetMousePosition(), r_baze);

	auto cur = messages.begin();
	size_t id = 0;
	while (cur != messages.end()) {
		messages[id].tic++;
		if (Draw_m(id, r_baze)) {
			cur = messages.erase(cur);
		}
		else {
			cur++;
			id++;
		}
	}
}

void NotifService::AddMessage(MType type, const std::string& name, const std::string& text) {
	Message msg;

	msg.name = name;

	for (char& c : msg.name) {
		if (c == '\n' || c == '\r') c = ' ';
	}

	msg.text = text;
	msg.type = type;
	msg.tic = 0;

	messages.push_back(msg);
}

void NotifService::ClearAll() {
	messages.clear();
}

bool NotifService::isHover() {
	if (messages.empty()) {
		return false;
	}
	return isHoverCursor;
}