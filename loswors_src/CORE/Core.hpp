#pragma once
#include <cstdint>

#include "Const.hpp"
#include "lwrs/LwrsFileUnit.hpp"

enum class UI_state {
	Intro,
	Start,
	Main
};
enum class InsideUIState {
	NONE,
	S_ST_CREATE_NEW,	// start state create new
	M_ST_PIN_CONFIRM,	// main state Enter PIN
	M_ST_CHANGE_FPARAM,	// main state Change file param
	M_ST_COPY_F,		// main state Copy file
	M_ST_EDIT_PU		// main state Edit PIN
};

enum class DecodeStatus {
	BinShuffle,
	FileHash,
	Argon2,
	G_XOR_Shuffle,
	I_XOR_Shuffle
};

struct UI_ctx {
	UI_state ui_state = UI_state::Intro;
	int Width = Const::WIDTH_DEF;
	int Height = Const::HEIGHT_DEF;
};


class Core {
public:
	Core() = delete;

//      ___                        
//     | _ \__ _ _ _ __ _ _ __  ___
//     |  _/ _` | '_/ _` | '  \(_-<
//     |_| \__,_|_| \__,_|_|_|_/__/
// ---------------------------------------------------------------------------
	static char MASTER_PASS[Const::MASTER_PASS_SIZE + 1];

	static char ENTER_OLD_MASTER_PASS[Const::MASTER_PASS_SIZE + 1];

	static uint8_t PIN[4];
	static uint8_t NEW_PIN[4];

	static bool IS_2_FILE_TEMP;

	static char NEW_NAME[Const::MAX_NEW_NAME_LEN + 1];

	static char NEW_PATH_0[Const::MAX_PATH_LEN + 1];
	static char NEW_PATH_1[Const::MAX_PATH_LEN + 1];

	static char NEW_MASTER_PASS_ENTER[Const::MASTER_PASS_SIZE + 1];
	static char NEW_MASTER_PASS_CONFIRM[Const::MASTER_PASS_SIZE + 1];

	static PU curPU;
	
	// FROM UI CALL CLEAR ---------------------------------------------------------

	static void ClearNEW_NAME();
	static void ClearNEW_PATH_0();
	static void ClearNEW_PATH_1();
	//static void ClearMASTER_PASS(); // not need for UI
	static void ClearENTER_OLD_MASTER_PASS();
	static void ClearNEW_MASTER_PASS_ENTER();
	static void ClearNEW_MASTER_PASS_CONFIRM();



	static void Enter_CREATE_NEW();
	static void Exit_CREATE_NEW();

	static void Enter_PIN_CONFIRM_SAVE();
	static void Exit_PIN_CONFIRM_SAVE();

	static void Enter_CHANGE_FPARAM();
	static void Exit_CHANGE_FPARAM();

	static void Enter_COPY_F();
	static void Exit_COPY_F();
	
	static void Enter_EDIT_PU(int floatID); // auto fill Core::curPU if need new -> floatID = -1
	static void Exit_SAVE_EDIT_PU(); // if new add if old change
	static void Exit_EDIT_PU();




	// for test on other size only !!!
	static void RESIZE(int w, int h); // mb keep, for settings window
	static void PLAY_INTRO_FROM_VERSION();



	static UI_ctx GetUI_ctx();
	static InsideUIState GetUIInsideState();

	static bool GetIsSaveStatus();
	static void SetSaveStatusFALSE();

	static void DROPFRAME_ENCODE(DecodeStatus status);

	static void END_INTRO();
	static void EXIT_TO_START(); // exit 100% | even if there is unsaved data


	static bool Try_CREATE_NEW_FILE();
	static bool Try_ENTER();
	static bool Try_SAVE();
	static bool Try_REMAKE_FILE();
	static bool Try_COPY_FILE();



	static void _INIT_();
	static void _UNINIT_();
};