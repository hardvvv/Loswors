#pragma once
#include <cstdint>
#include <raylib.h>

#include "../CORE/Core.hpp"

/*
	----------------------- VISUAL -----------------------

	The entire visual part of the program will implement the following logic:

	The UI is designed so that if you change the window size in Const,
	the entire UI will adjust as if it were a stretched version of the previous state
	(This looks strange at critical values)

	The variables represent the percentage of the size relative to the total available size, where 0 is 0% and 1 is 100%
	prefix _x0 | _y0 -> relative position of the drawing start
	prefix _x1 | _y1 -> relative size of the drawing area
	If a value requires a relationship to the minimum side, it has a "_wh" prefix

	Sometimes there are minor local changes in the rendering functions
*/
enum class FS_Panel : uint8_t {
	NONE,
	LOG,
	VERSION,
	SETTINGS
};

enum class HeaderTextState {
	NONE,
	LOG,
	VERSION,
	SETTINGS,
	CREATE_NEW_FILE,
	ENTER_DATA_PASS_UNIT,
	EDIT_FILE_SETTINGS,
	COPY_FILE,
	PIN_VERIF				// need to add TEXT
};


class UI {
private:

	static FS_Panel fs_panel_status;

	static HeaderTextState header_text_status;

	

public:
	UI() = delete;

	// use from .cpp !
	static FS_Panel Get_FSP();
	static void Set_FSP(FS_Panel status);
	static void Set_HeaderTextStatus(HeaderTextState status);
	static HeaderTextState Get_HeaderTextStatus();

	static float GetTextSize();
	static float GetMinWH();

	static void ResetMain();

	static void DropFrameDecode(DecodeStatus status); // from core call

	static void RenderFrame(UI_ctx ctx);
	static void StartLoop();


	static void _INIT_();
	static void _UNINIT_();
};