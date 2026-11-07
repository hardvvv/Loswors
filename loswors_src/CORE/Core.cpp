#include "Core.hpp"

#include <string>
#include <format>
#include <algorithm>

#include "license_s.hpp"

#include "../UI/UI.hpp"
#include "../UI/NotifService.hpp"
#include "Log.hpp"
#include "SecureWipe.hpp"
#include "lwrs/LwrsFileUnit.hpp"
#include "FS/FileSys.hpp"
#include "Encode/UTF8_to_NFC.hpp"

#include "Settings.hpp"

#include "../Localization/LocalSys.hpp"
#include "../Localization/loc.hpp"


// FT - (format text)
namespace {
	// ltxt - (Log text)
	namespace ltxt {
		namespace dropframe {
			constexpr char DS_BinShuffle[]	= "[ Lwrs ] >> Bin Shuffle";
			constexpr char DS_FileHash[]	= "[ Lwrs ] >> File Hash";
			constexpr char DS_Argon2[]		= "[ Lwrs ] >> Argon2";
			constexpr char DS_GXORShuffle[] = "[ Lwrs ] >> Global XOR Shuffle";
			constexpr char DS_IXORShuffle[] = "[ Lwrs ] >> Inside Pass Units XOR Shuffle";
		}
		// ss - (safe save)
		namespace ss {
			constexpr char OldDelDone[]			= "[ Save Sys ] >> Old file delete DONE";
			constexpr char OldDelErr_FT[]		= "[ Save Sys ] >> Old file delete Error: {}";

			constexpr char TmpRnmDone[]			= "[ Save Sys ] >> TEMP file rename DONE";
			constexpr char TmpRnmErr_FT[]		= "[ Save Sys ] >> TEMP file rename Error: {}";

			constexpr char TmpFirstDelDone[]	= "[ Save Sys ] >> TEMP First file delete DONE";
			constexpr char TmpFirstDelErr_FT[]	= "[ Save Sys ] >> TEMP First file delete Error: {}";

			constexpr char FirstDelDone[]		= "[ Save Sys ] >> First file delete DONE";
			constexpr char FirstDelErr_FT[]		= "[ Save Sys ] >> First file delete Error: {}";

			constexpr char OkCreateTmp_FT[]		= "[ Save Sys ] >> Success create TEMP file at path: {}";
			constexpr char ErrCreateTmp_FT[]	= "[ Save Sys ] >> Error Write Data To TEMP File | Error: {}";

			constexpr char OkCreate_FT[]		= "[ Save Sys ] >> Successfully created file at path: {}";
			constexpr char ErrCreate_FT[]		= "[ Save Sys ] >> Error Write Data To File | Error: {}";
		}
		// path combine
		namespace PthComb {
			constexpr char NoDir[]				= "[ Save Sys ] >> Dir for new save file NOT exist";
			constexpr char FileNameColizion[]	= "[ Save Sys ] >> Save file whith same name allready exist";
		}
		
		namespace EnterErr {
			constexpr char Read_2FT[]		= "[ Try Enter ] >> Error ({}) while read file {}";
			constexpr char DownVersion[]	= "[ Try Enter ] >> Down file Version!";
			constexpr char UperVersion[]	= "[ Try Enter ] >> Error (Uper File Version) / (File Format)";
			constexpr char Size[]			= "[ Try Enter ] >> Error file Size";
			constexpr char Format[]			= "[ Try Enter ] >> Error file Format";
			constexpr char Argon2[]			= "[ Try Enter ] >> ARGON2 Error";
		}
		// rf - (return false)
		namespace rf {
			constexpr char PinHashErr[]			= "[ Input Verif ] >> Incorrect file PIN";
			constexpr char PinNonASCII_FT[]		= "[ Input Verif ] >> PIN not ASCII symbol at index: {}";
			constexpr char NewPinNonASCII_FT[]	= "[ Input Verif ] >> New PIN not ASCII symbol at index: {}";

			constexpr char OldPassInvalid[]		= "[ Input Verif ] >> Old password incorrect";
			constexpr char NewPassNotsame_FT[]	= "[ Input Verif ] >> New password not match at index: {}";
		}

		constexpr char ChangeLost[]			= "You exit without save | Changes Lost";

		constexpr char EndInit[]			= "[ CORE ] >> End INIT <<";

		constexpr char EnterMain[]			= "[ Core ] >> Enter Main state";

		constexpr char SaveLicenseOK_FT[]	= "[ Core ] >> License file successfully created at path: {}";
	}
}

namespace {
	bool is_Save = true;
	UI_ctx ui_context = UI_ctx{};
	InsideUIState i_ui_st = InsideUIState::NONE;
}

//      ___                        
//     | _ \__ _ _ _ __ _ _ __  ___
//     |  _/ _` | '_/ _` | '  \(_-<
//     |_| \__,_|_| \__,_|_|_|_/__/
// ------------------------------------------------------------------

char Core::MASTER_PASS[Const::MASTER_PASS_SIZE + 1] = {};

char Core::ENTER_OLD_MASTER_PASS[Const::MASTER_PASS_SIZE + 1] = {};

uint8_t Core::PIN[4] = {
	static_cast<uint8_t>('0'),
	static_cast<uint8_t>('0'),
	static_cast<uint8_t>('0'),
	static_cast<uint8_t>('0')
};
uint8_t Core::NEW_PIN[4] = {
	static_cast<uint8_t>('0'),
	static_cast<uint8_t>('0'),
	static_cast<uint8_t>('0'),
	static_cast<uint8_t>('0')
};

bool Core::IS_2_FILE_TEMP = false;

char Core::NEW_NAME[Const::MAX_NEW_NAME_LEN + 1] = {};

char Core::NEW_PATH_0[Const::MAX_PATH_LEN + 1] = {};
char Core::NEW_PATH_1[Const::MAX_PATH_LEN + 1] = {};

char Core::NEW_MASTER_PASS_ENTER[Const::MASTER_PASS_SIZE + 1] = {};
char Core::NEW_MASTER_PASS_CONFIRM[Const::MASTER_PASS_SIZE + 1] = {};

PU Core::curPU = PU{};

// param handlers - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace {

	inline void Clear_MASTER_PASS() {
		SecureWipe(Core::MASTER_PASS, Const::MASTER_PASS_SIZE + 1);
	}
	inline void Clear_ENTER_OLD_MASTER_PASS() {
		SecureWipe(Core::ENTER_OLD_MASTER_PASS, Const::MASTER_PASS_SIZE + 1);
	}
	inline void Clear_PIN() {
		for (int i = 0; i < 4; i++) {
			Core::PIN[i] = static_cast<uint8_t>('0');
		}
	}
	inline void Clear_NEW_PIN() {
		for (int i = 0; i < 4; i++) {
			Core::NEW_PIN[i] = static_cast<uint8_t>('0');
		}
	}
	inline void Clear_NEW_NAME() {
		SecureWipe(Core::NEW_NAME, Const::MAX_NEW_NAME_LEN + 1);
	}
	inline void Clear_NEW_PATH_0() {
		SecureWipe(Core::NEW_PATH_0, Const::MAX_PATH_LEN + 1);
	}
	inline void Clear_NEW_PATH_1() {
		SecureWipe(Core::NEW_PATH_1, Const::MAX_PATH_LEN + 1);
	}
	inline void Clear_NEW_MASTER_PASS_ENTER() {
		SecureWipe(Core::NEW_MASTER_PASS_ENTER, Const::MASTER_PASS_SIZE + 1);
	}
	inline void Clear_NEW_MASTER_PASS_CONFIRM() {
		SecureWipe(Core::NEW_MASTER_PASS_CONFIRM, Const::MASTER_PASS_SIZE + 1);
	}

	inline void Clear_ALL() {
		Clear_MASTER_PASS();
		Clear_ENTER_OLD_MASTER_PASS();
		Clear_PIN();
		Clear_NEW_PIN();
		Clear_NEW_NAME();
		Clear_NEW_PATH_0();
		Clear_NEW_PATH_1();
		Clear_NEW_MASTER_PASS_ENTER();
		Clear_NEW_MASTER_PASS_CONFIRM();
	}

};

//      _   _ ___   _  _              _ _            
//     | | | |_ _| | || |__ _ _ _  __| | |___ _ _ ___
//     | |_| || |  | __ / _` | ' \/ _` | / -_) '_(_-<
//      \___/|___| |_||_\__,_|_||_\__,_|_\___|_| /__/
// ------------------------------------------------------------------

void Core::ClearNEW_NAME() {
	Clear_NEW_NAME();
}
void Core::ClearNEW_PATH_0() {
	Clear_NEW_PATH_0();
}
void Core::ClearNEW_PATH_1() {
	Clear_NEW_PATH_1();
}
void Core::ClearENTER_OLD_MASTER_PASS() {
	Clear_ENTER_OLD_MASTER_PASS();
}
void Core::ClearNEW_MASTER_PASS_ENTER() {
	Clear_NEW_MASTER_PASS_ENTER();
}
void Core::ClearNEW_MASTER_PASS_CONFIRM() {
	Clear_NEW_MASTER_PASS_CONFIRM();
}


// SET status & clear data
void Core::Enter_CREATE_NEW() {
	i_ui_st = InsideUIState::S_ST_CREATE_NEW;
}
void Core::Exit_CREATE_NEW() {
	Clear_MASTER_PASS();
	Clear_NEW_PIN();
	Clear_NEW_NAME();
	Clear_NEW_PATH_0();
	Clear_NEW_PATH_1();
	Clear_NEW_MASTER_PASS_ENTER();
	Clear_NEW_MASTER_PASS_CONFIRM();
	i_ui_st = InsideUIState::NONE;
}
void Core::Enter_PIN_CONFIRM_SAVE() {
	if (!is_Save) {
		i_ui_st = InsideUIState::M_ST_PIN_CONFIRM;
	}
}
void Core::Exit_PIN_CONFIRM_SAVE() {
	Clear_PIN();
	i_ui_st = InsideUIState::NONE;
}
void Core::Enter_CHANGE_FPARAM() {
	if (!is_Save) {
		NotifService::AddMessage(MType::M_WARNING, loc::t.notif.core.unsavewarn0, loc::t.notif.core.unsavewarn1);
		return;
	}
	if (i_ui_st == InsideUIState::M_ST_COPY_F) {
		Exit_COPY_F();
	}
	i_ui_st = InsideUIState::M_ST_CHANGE_FPARAM;
}
void Core::Exit_CHANGE_FPARAM() {
	Clear_ENTER_OLD_MASTER_PASS();
	Clear_PIN();
	Clear_NEW_PIN();
	Clear_NEW_MASTER_PASS_ENTER();
	Clear_NEW_MASTER_PASS_CONFIRM();
	i_ui_st = InsideUIState::NONE;
}
void Core::Enter_COPY_F() {
	if (!is_Save) {
		NotifService::AddMessage(MType::M_WARNING, loc::t.notif.core.unsavewarn0, loc::t.notif.core.unsavewarn1);
		return;
	}
	if (i_ui_st == InsideUIState::M_ST_CHANGE_FPARAM) {
		Exit_CHANGE_FPARAM();
	}
	IS_2_FILE_TEMP = Settings::is_2file;
	i_ui_st = InsideUIState::M_ST_COPY_F;
}
void Core::Exit_COPY_F() {
	Clear_ENTER_OLD_MASTER_PASS();
	Clear_PIN();
	Clear_NEW_PIN();
	Clear_NEW_NAME();
	Clear_NEW_PATH_0();
	Clear_NEW_PATH_1();
	Clear_NEW_MASTER_PASS_ENTER();
	Clear_NEW_MASTER_PASS_CONFIRM();
	i_ui_st = InsideUIState::NONE;
}
void Core::Enter_EDIT_PU(int floatID) {
	if (floatID < 0 || floatID >= LwrsFileUnit::GetPUCount()) {
		// new Unit
		curPU.Clear();
		curPU.ConstID = -1;
	}
	else {
		// normal edit curent
		curPU = LwrsFileUnit::GetPUByfloatID(floatID);
	}
	i_ui_st = InsideUIState::M_ST_EDIT_PU;
}
void Core::Exit_SAVE_EDIT_PU() {
	if (curPU.ConstID < 0 || curPU.ConstID >= LwrsFileUnit::GetPUCount()) {
		// new
		LwrsFileUnit::AddPU(curPU.name, curPU.pass, curPU.desc);
	}
	else {
		// change
		LwrsFileUnit::ChangePU(curPU.ConstID, curPU.name, curPU.pass, curPU.desc);
	}
	is_Save = false;
	Exit_EDIT_PU();
}
void Core::Exit_EDIT_PU() {
	curPU.Clear();
	curPU.ConstID = -1;

	i_ui_st = InsideUIState::NONE;
}




void Core::RESIZE(int w, int h) {
	// mb this incorect to be in CORE... Need to be in UI (but not now...)

	int prevW = ui_context.Width;
	int prevH = ui_context.Height;

	ui_context.Width = std::clamp(w, Const::WIDTH_MIN, Const::WIDTH_MAX);
	ui_context.Height = std::clamp(h, Const::HEIGHT_MIN, Const::HEIGHT_MAX) + Const::WIN_HEADER_PX;

	SetWindowSize(ui_context.Width, ui_context.Height);

	Vector2 currentPos = GetWindowPosition();
	int currentMonitor = GetCurrentMonitor();

	int monitorX = GetMonitorPosition(currentMonitor).x;
	int monitorY = GetMonitorPosition(currentMonitor).y;
	int monitorW = GetMonitorWidth(currentMonitor);
	int monitorH = GetMonitorHeight(currentMonitor);

	int targetX = static_cast<int>(std::round(currentPos.x - (ui_context.Width - prevW) * 0.5f));
	int targetY = static_cast<int>(std::round(currentPos.y - (ui_context.Height - prevH) * 0.5f));

	int newX = std::clamp(targetX, monitorX, monitorX + monitorW - ui_context.Width);
	int newY = std::clamp(targetY, monitorY, monitorY + monitorH - ui_context.Height);

	SetWindowPosition(newX, newY);
}

static UI_state ui_state_befor_version_intro = UI_state::Start;
static bool isINTRO_FROM_VERSION = false;
void Core::PLAY_INTRO_FROM_VERSION() {
	isINTRO_FROM_VERSION = true;
	UI::Set_FSP(FS_Panel::NONE);
	ui_state_befor_version_intro = ui_context.ui_state;
	ui_context.ui_state = UI_state::Intro;
}




UI_ctx Core::GetUI_ctx() {
	return ui_context;
}
InsideUIState Core::GetUIInsideState() {
	return i_ui_st;
}

bool Core::GetIsSaveStatus() {
	return is_Save;
}
void Core::SetSaveStatusFALSE() {
	is_Save = false;
}

void Core::DROPFRAME_ENCODE(DecodeStatus status) {
	switch (status)
	{
	case DecodeStatus::BinShuffle:
		Log::add(ltxt::dropframe::DS_BinShuffle, L_INFO);
		break;
	case DecodeStatus::FileHash:
		Log::add(ltxt::dropframe::DS_FileHash, L_INFO);
		break;
	case DecodeStatus::Argon2:
		Log::add(ltxt::dropframe::DS_Argon2, L_INFO);
		break;
	case DecodeStatus::G_XOR_Shuffle:
		Log::add(ltxt::dropframe::DS_GXORShuffle, L_INFO);
		break;
	case DecodeStatus::I_XOR_Shuffle:
		Log::add(ltxt::dropframe::DS_IXORShuffle, L_INFO);
		break;
	default:
		return;
	}
	UI::DropFrameDecode(status);
}

void Core::END_INTRO() {
	if (isINTRO_FROM_VERSION) {
		ui_context.ui_state = ui_state_befor_version_intro;
		UI::Set_FSP(FS_Panel::VERSION);
		return;
	}
	ui_context.ui_state = UI_state::Start;
}
void Core::EXIT_TO_START() {
	LwrsFileUnit::ClearSelf();
	Clear_ALL();

	if (ui_context.ui_state == UI_state::Main) {
		
		switch (i_ui_st)
		{
		case InsideUIState::M_ST_PIN_CONFIRM:
			Exit_PIN_CONFIRM_SAVE();
			break;
		case InsideUIState::M_ST_CHANGE_FPARAM:
			Exit_CHANGE_FPARAM();
			break;
		case InsideUIState::M_ST_COPY_F:
			Exit_COPY_F();
			break;
		case InsideUIState::M_ST_EDIT_PU:
			Exit_EDIT_PU();
			break;
		default:
			break;
		}

		if (!is_Save) {
			Log::add(ltxt::ChangeLost, L_WARNING);
			NotifService::AddMessage(MType::M_WARNING, loc::t.notif.core.changelost0, loc::t.notif.core.changelost1);
		}
		ui_context.ui_state = UI_state::Start;
		UI::ResetMain();
	}
	if (ui_context.ui_state == UI_state::Start) {

		if (i_ui_st == InsideUIState::S_ST_CREATE_NEW) {
			Exit_CREATE_NEW();
		}
	}
	
	is_Save = true;
}



//      _                   ___ _ _      __      __       _   
//     | | __ __ ___ _ ___ | __(_) |___  \ \    / /__ _ _| |__
//     | |_\ V  V / '_(_-< | _|| | / -_)  \ \/\/ / _ \ '_| / /
//     |____\_/\_/|_| /__/ |_| |_|_\___|   \_/\_/\___/_| |_\_\
// ------------------------------------------------------------------
namespace {

	inline bool isPIN_ASCII(int& errorID, const uint8_t(&PIN)[4]) {
		for (int i = 0; i < 4; ++i) {
			// ASCII printable
			if (PIN[i] < 32 || PIN[i] > 126) {
				errorID = i;
				return false;
			}
		}
		return true;
	}

	inline bool isPASSCONFIRM(int& errorID, const uint8_t* P0, size_t P0sz, const uint8_t* P1, size_t P1sz) {
		if (!P0 || !P1 || P0sz != P1sz || P0sz == 0 || P1sz == 0) return false;

		for (int i = 0; i < P0sz; ++i) {
			if (P0[i] != P1[i]) {
				errorID = i;
				return false;
			}
		}
		return true;
	}

	// no check
	std::string GetTEMP_PATH_utf8(const std::string& path_utf8) {
		std::string result = path_utf8;

		size_t dot_pos = result.rfind('.');

		if (dot_pos != std::string::npos) {
			result.resize(dot_pos + 1);
		}
		else {
			result += '.';
		}

		result += Const::LOSWORS_TEMP_FILE_FORMAT;

		return result;
	}

	/*
		this func. DONT check the path !(need to do it before call)
		this func. DONT reset the LwrsFileUnit
		this func. only add LOG
	*/
	bool SafeSaveBy(
		bool isSafeSave, bool is2file,
		const std::string& path0_utf8, const std::string& path1_utf8,
		const uint8_t(&PIN)[4], const uint8_t* PASS, size_t PASS_size
	) {
		// ---------------------------------
		//		Temp File data
		// --------------------------------------------------------------------------------------------
		uint8_t* TEMP_GFILE_DATA = nullptr;
		size_t TEMP_GFILE_DATA_SIZE = 0;

		uint8_t* TEMP_D0 = nullptr;
		uint8_t* TEMP_D1 = nullptr;
		size_t TEMP_D0_SIZE = 0;
		size_t TEMP_D1_SIZE = 0;

		auto clearTEMP = [&]() {
			SecureWipe(TEMP_GFILE_DATA, TEMP_GFILE_DATA_SIZE);
			delete[] TEMP_GFILE_DATA; TEMP_GFILE_DATA = nullptr; TEMP_GFILE_DATA_SIZE = 0;

			SecureWipe(TEMP_D0, TEMP_D0_SIZE);
			SecureWipe(TEMP_D1, TEMP_D1_SIZE);
			delete[] TEMP_D0; TEMP_D0 = nullptr; TEMP_D0_SIZE = 0;
			delete[] TEMP_D1; TEMP_D1 = nullptr; TEMP_D1_SIZE = 0;	
		};

		auto NOTEMP = [&](const std::string& NormPath) {
			if (!isSafeSave) return;

			std::string TEMPpath = GetTEMP_PATH_utf8(NormPath);

			FileSys::Status st = FileSys::delete_file(NormPath);
			(st == FileSys::Status::Success) ?
				Log::add(ltxt::ss::OldDelDone, L_INFO) :
				Log::add(std::format(ltxt::ss::OldDelErr_FT, FileSys::GetStatusTEXTen(st)), L_ERROR);

			st = FileSys::rename_file(TEMPpath, NormPath);
			(st == FileSys::Status::Success) ?
				Log::add(ltxt::ss::TmpRnmDone, L_OK) :
				Log::add(std::format(ltxt::ss::TmpRnmErr_FT, FileSys::GetStatusTEXTen(st)), L_ERROR);
		};
		auto DEL_F = [&](const std::string& NormPath) {
			
			if (isSafeSave) {
				std::string TEMPpath = GetTEMP_PATH_utf8(NormPath);

				FileSys::Status del_st = FileSys::delete_file(TEMPpath);
				(del_st == FileSys::Status::Success) ?
					Log::add(ltxt::ss::TmpFirstDelDone, L_WARNING) :
					Log::add(std::format(ltxt::ss::TmpFirstDelErr_FT, FileSys::GetStatusTEXTen(del_st)), L_ERROR);
				return;
			}

			FileSys::Status del_st = FileSys::delete_file(NormPath);
			(del_st == FileSys::Status::Success) ?
				Log::add(ltxt::ss::FirstDelDone, L_WARNING) :
				Log::add(std::format(ltxt::ss::FirstDelErr_FT, FileSys::GetStatusTEXTen(del_st)), L_ERROR);
		};

		auto F_Write = [&](const std::string& path_utf8, const uint8_t* DATA, size_t DATAsz) -> bool {

			if (isSafeSave) {
				std::string TEMPpath = GetTEMP_PATH_utf8(path_utf8);

				FileSys::Status tst = FileSys::write_bytes_to_file(TEMPpath, DATA, DATAsz);
				if (tst == FileSys::Status::Success) {
					Log::add(std::format(ltxt::ss::OkCreateTmp_FT, TEMPpath), L_INFO);
					return true;
				}
				else {
					Log::add(std::format(ltxt::ss::ErrCreateTmp_FT, FileSys::GetStatusTEXTen(tst)), L_ERROR);
					return false;
				}
				return true;
			}

			FileSys::Status st = FileSys::write_bytes_to_file(path_utf8, DATA, DATAsz);
			if (st == FileSys::Status::Success) {
				Log::add(std::format(ltxt::ss::OkCreate_FT, path_utf8), L_OK);
				return true;
			}
			else {
				Log::add(std::format(ltxt::ss::ErrCreate_FT, FileSys::GetStatusTEXTen(st)), L_ERROR);
				return false;
			}
			return true;
		};

		// ---------------------------------
		//		Get Save Data
		// --------------------------------------------------------------------------------------------

		TEMP_GFILE_DATA = LwrsFileUnit::GetDataForSave(TEMP_GFILE_DATA_SIZE, PIN, PASS, PASS_size);

		// ---------------------------------
		//		If Need BIN SHUFFLE    &&    SAVE IN FILE
		// --------------------------------------------------------------------------------------------
		bool ret = false;
		if (is2file) {
			// 2 file - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			LwrsFileUnit::DATA_DOUBLE(
				TEMP_GFILE_DATA, TEMP_GFILE_DATA_SIZE,
				TEMP_D0, TEMP_D0_SIZE,
				TEMP_D1, TEMP_D1_SIZE
			);

			do {
				if (!F_Write(path0_utf8, TEMP_D0, TEMP_D0_SIZE)) {
					break;
				}

				if (!F_Write(path1_utf8, TEMP_D1, TEMP_D1_SIZE)) {
					DEL_F(path0_utf8);
					break;
				}

				NOTEMP(path0_utf8);
				NOTEMP(path1_utf8);
				ret = true;

			} while (false);
		}
		else {
			// 1 file - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			if (F_Write(path0_utf8, TEMP_GFILE_DATA, TEMP_GFILE_DATA_SIZE)) {
				NOTEMP(path0_utf8);
				ret = true;
			}
		}

		clearTEMP();
		return ret;
	}


	// LOCAL
	bool PathCombName(std::string& toUPDATE, std::string DIR, std::string NAME) {
		if (!FileSys::Ping_DirExists(DIR)) {
			Log::add(ltxt::PthComb::NoDir, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR, loc::t.notif.core.sv.errdirnotexist0, loc::t.notif.core.sv.errdirnotexist1);
			return false;
		}
		toUPDATE = FileSys::PATH_COMBINE_utf8(DIR, std::format("{}.{}", NAME, Const::LOSWORS_FILE_FORMAT));

		if (FileSys::Ping_FileExists(toUPDATE)) {
			Log::add(ltxt::PthComb::FileNameColizion, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR, loc::t.notif.core.sv.errfileexis0, loc::t.notif.core.sv.errfileexis1);
			return false;
		}
		return true;
	}
	bool Path2CombName(std::string& toUPDATE0, std::string& toUPDATE1, std::string DIR0, std::string DIR1, std::string NAME) {
		if (!FileSys::Ping_DirExists(DIR0) || !FileSys::Ping_DirExists(DIR1)) {
			Log::add(ltxt::PthComb::NoDir, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR, loc::t.notif.core.sv.errdirnotexist0, loc::t.notif.core.sv.errdirnotexist1);
			return false;
		}
		toUPDATE0 = FileSys::PATH_COMBINE_utf8(DIR0, std::format("{}_0.{}", NAME, Const::LOSWORS_FILE_FORMAT));
		toUPDATE1 = FileSys::PATH_COMBINE_utf8(DIR1, std::format("{}_1.{}", NAME, Const::LOSWORS_FILE_FORMAT));

		if (FileSys::Ping_FileExists(toUPDATE0) || FileSys::Ping_FileExists(toUPDATE1)) {
			Log::add(ltxt::PthComb::FileNameColizion, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR, loc::t.notif.core.sv.errfileexis0, loc::t.notif.core.sv.errfileexis1);
			return false;
		}
		return true;
	}

}

bool Core::Try_CREATE_NEW_FILE() {
	// ---------------------------------
	//		Temp File data
	// --------------------------------------------------------------------------------------------
	std::string TEMP_path0;
	std::string TEMP_path1;

	constexpr size_t TXT_SIZE = Const::MASTER_PASS_SIZE + 1;
	constexpr size_t BYT_SIZE = Const::MASTER_PASS_SIZE;

	uint8_t TEMP_PASS_ENTER[BYT_SIZE] = {};
	uint8_t TEMP_PASS_CONFIRM[BYT_SIZE] = {};

	uint8_t* TEMP_GFILE_DATA = nullptr;
	size_t TEMP_GFILE_DATA_SIZE = 0;

	uint8_t* TEMP_D0 = nullptr;
	uint8_t* TEMP_D1 = nullptr;
	size_t TEMP_D0_SIZE = 0;
	size_t TEMP_D1_SIZE = 0;

	auto clearTEMP = [&]() {
		TEMP_path0.clear();		TEMP_path0.shrink_to_fit();
		TEMP_path1.clear();		TEMP_path1.shrink_to_fit();
		SecureWipe(TEMP_PASS_ENTER, BYT_SIZE);
		SecureWipe(TEMP_PASS_CONFIRM, BYT_SIZE);
		
		SecureWipe(TEMP_GFILE_DATA, TEMP_GFILE_DATA_SIZE);
		delete[] TEMP_GFILE_DATA; TEMP_GFILE_DATA = nullptr;
		TEMP_GFILE_DATA_SIZE = 0;

		SecureWipe(TEMP_D0, TEMP_D0_SIZE);
		SecureWipe(TEMP_D1, TEMP_D1_SIZE);
		delete[] TEMP_D0; TEMP_D0 = nullptr;
		delete[] TEMP_D1; TEMP_D1 = nullptr;
		TEMP_D0_SIZE = 0;
		TEMP_D1_SIZE = 0;
	};

	auto BeforRetFALSE = [&](const std::string& log, const std::string& NotifHeader, const std::string& NotifBoby) {
		Log::add(log, L_ERROR);
		NotifService::AddMessage(MType::M_ERROR, NotifHeader, NotifBoby);
		clearTEMP();
	};

	auto CopyToSettings = [&](bool isFIRST_FILE) {
		if (isFIRST_FILE) {
			Settings::ClearFPath0(); Settings::ClearFPath1();
			snprintf(Settings::path_f0, sizeof(Settings::path_f0), "%s", TEMP_path0.c_str());
		}
		else {
			Settings::ClearFPath1();
			snprintf(Settings::path_f1, sizeof(Settings::path_f1), "%s", TEMP_path1.c_str());
		}
	};

	// ---------------------------------
	//		Path Combine & Check
	// --------------------------------------------------------------------------------------------
	// log & notif inside PathCombName
	if (IS_2_FILE_TEMP) {
		if (!Path2CombName(TEMP_path0, TEMP_path1, NEW_PATH_0, NEW_PATH_1, NEW_NAME)) return false;
	}
	else {
		if (!PathCombName(TEMP_path0, NEW_PATH_0, NEW_NAME)) return false;
	}

	// ---------------------------------
	//		Check PASS
	// --------------------------------------------------------------------------------------------
	SecureWipe(TEMP_PASS_ENTER, BYT_SIZE);
	SecureWipe(TEMP_PASS_CONFIRM, BYT_SIZE);
	UTF8_to_NFC::to_nfc_bytes(NEW_MASTER_PASS_ENTER,	TXT_SIZE, TEMP_PASS_ENTER,		BYT_SIZE);
	UTF8_to_NFC::to_nfc_bytes(NEW_MASTER_PASS_CONFIRM,	TXT_SIZE, TEMP_PASS_CONFIRM,	BYT_SIZE);

	int PASS_erID = 0;
	if (!isPASSCONFIRM(PASS_erID, TEMP_PASS_ENTER, BYT_SIZE, TEMP_PASS_CONFIRM, BYT_SIZE)) {
		BeforRetFALSE(std::format(ltxt::rf::NewPassNotsame_FT, PASS_erID),
			loc::t.notif.core.inptverf.newpasserr0, loc::t.notif.core.inptverf.newpasserr1);
		return false;
	}

	// ---------------------------------
	//		Check PIN
	// --------------------------------------------------------------------------------------------
	int PINerID = 0;
	if (!isPIN_ASCII(PINerID, NEW_PIN)) {
		BeforRetFALSE(std::format(ltxt::rf::PinNonASCII_FT, PINerID),
			loc::t.notif.core.inptverf.newpinasciierr0, loc::t.notif.core.inptverf.newpinasciierr1);
		return false;
	}

	// ---------------------------------
	//		Clear & Save Data
	// --------------------------------------------------------------------------------------------
	LwrsFileUnit::ClearSelf();

	bool ret = SafeSaveBy(false, IS_2_FILE_TEMP, TEMP_path0, TEMP_path1, NEW_PIN, TEMP_PASS_ENTER, BYT_SIZE);

	if (ret) {
		NotifService::AddMessage(MType::M_NORMAL, loc::t.notif.core.sv.crtdone0, loc::t.notif.core.sv.crtdone1);
		Exit_CREATE_NEW(); // already make all clear inside
		if (IS_2_FILE_TEMP) {
			CopyToSettings(true);
			CopyToSettings(false);
		}
		else {
			CopyToSettings(true);
		}
		Settings::is_2file = IS_2_FILE_TEMP;
	}
	else {
		NotifService::AddMessage(MType::M_ERROR, loc::t.notif.core.sv.errsave0, loc::t.notif.core.sv.errsave1);
	}

	clearTEMP();
	return ret;
}

bool Core::Try_ENTER() {
	LwrsFileUnit::ClearSelf();
	// ---------------------------------
	//		Temp
	// --------------------------------------------------------------------------------------------
	uint8_t TEMP_PASS_NFC[Const::MASTER_PASS_SIZE] = {}; SecureWipe(TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);
	UTF8_to_NFC::to_nfc_bytes(MASTER_PASS, Const::MASTER_PASS_SIZE + 1, TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);

	uint8_t* TEMP_D0 = nullptr;
	uint8_t* TEMP_D1 = nullptr;
	size_t TEMP_D0_SIZE = 0;
	size_t TEMP_D1_SIZE = 0;

	auto clearTEMP = [&]() {
		SecureWipe(TEMP_D0, TEMP_D0_SIZE);
		delete[] TEMP_D0; TEMP_D0 = nullptr;
		SecureWipe(TEMP_D1, TEMP_D1_SIZE);
		delete[] TEMP_D1; TEMP_D1 = nullptr;

		SecureWipe(TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);
	};

	auto TryRead = [&](char* path, uint8_t*& DataToFill, size_t& NewSize, const char* f_tag = "") -> bool {

		FileSys::Status st = FileSys::read_bytes_from_file(path, DataToFill, NewSize);
		if (st != FileSys::Status::Success) {
			NotifService::AddMessage(MType::M_ERROR,
				loc::t.notif.core.tryentr.errfread0, loc::t.notif.core.tryentr.errfread1);
			Log::add(std::format(ltxt::EnterErr::Read_2FT, FileSys::GetStatusTEXTen(st), f_tag), L_ERROR);
			clearTEMP();
			return false;
		}
		return true;
	};

	auto isLwrsOK = [&](LwrsStatusOperation status) -> bool {
		if (status == LwrsStatusOperation::OK) {
			clearTEMP();
			return true;
		}
		else if (status == LwrsStatusOperation::DownVersion) {
			Log::add(ltxt::EnterErr::DownVersion, L_WARNING);
			NotifService::AddMessage(MType::M_WARNING,
				loc::t.notif.core.tryentr.warnfdownver0, loc::t.notif.core.tryentr.warnfdownver1);
			clearTEMP();
			return true;
		}
		
		if (status == LwrsStatusOperation::ErrorUpVersion) {
			Log::add(ltxt::EnterErr::UperVersion, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR,
				loc::t.notif.core.tryentr.errfupver0, loc::t.notif.core.tryentr.errfupver1);
		}
		else if (status == LwrsStatusOperation::ErrorSize) {
			Log::add(ltxt::EnterErr::Size, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR,
				loc::t.notif.core.tryentr.errfsize0, loc::t.notif.core.tryentr.errfsize1);
		}
		else if (status == LwrsStatusOperation::ErrorFormat) {
			Log::add(ltxt::EnterErr::Format, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR,
				loc::t.notif.core.tryentr.errfformat0, loc::t.notif.core.tryentr.errfformat1);
		}
		else if (status == LwrsStatusOperation::ErrorARGON2) {
			Log::add(ltxt::EnterErr::Argon2, L_ERROR);
			NotifService::AddMessage(MType::M_ERROR,
				loc::t.notif.core.tryentr.errargon0, loc::t.notif.core.tryentr.errargon1);
		}
		clearTEMP();
		return false;
	};

	// ---------------------------------
	//		Body
	// --------------------------------------------------------------------------------------------
	if (!Settings::is_2file) {
		// One file
		if (!TryRead(Settings::path_f0, TEMP_D0, TEMP_D0_SIZE)) return false;
		
		LwrsStatusOperation status = LwrsFileUnit::FillSelf(
			TEMP_D0, TEMP_D0_SIZE,
			TEMP_PASS_NFC, Const::MASTER_PASS_SIZE
		);

		if (!isLwrsOK(status)) return false;
	}
	else {
		// Two file
		if (!TryRead(Settings::path_f0, TEMP_D0, TEMP_D0_SIZE, "0 ")) return false;
		if (!TryRead(Settings::path_f1, TEMP_D1, TEMP_D1_SIZE, "1 ")) return false;

		LwrsStatusOperation status = LwrsFileUnit::FillSelf_DOUBLE(
			TEMP_D0, TEMP_D0_SIZE, 
			TEMP_D1, TEMP_D1_SIZE,
			TEMP_PASS_NFC, Const::MASTER_PASS_SIZE
		);

		if (!isLwrsOK(status)) return false;
	}


	ui_context.ui_state = UI_state::Main;
	i_ui_st = InsideUIState::NONE;
	Log::add(ltxt::EnterMain, L_OK);
	return true;
}

bool Core::Try_SAVE() {
	// ---------------------------------
	//		Temp
	// --------------------------------------------------------------------------------------------
	uint8_t TEMP_PASS_NFC[Const::MASTER_PASS_SIZE] = {}; SecureWipe(TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);
	UTF8_to_NFC::to_nfc_bytes(MASTER_PASS, Const::MASTER_PASS_SIZE + 1, TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);

	auto BeforRetFALSE = [&](const std::string& log, const std::string& NotifHeader, const std::string& NotifBoby) {
		Log::add(log, L_ERROR);
		NotifService::AddMessage(MType::M_ERROR, NotifHeader, NotifBoby);

		SecureWipe(TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);
	};

	// ---------------------------------
	//		PIN Check Hash
	// --------------------------------------------------------------------------------------------
	int PINerID = 0;
	if (!isPIN_ASCII(PINerID, PIN)) {
		BeforRetFALSE(std::format(ltxt::rf::PinNonASCII_FT, PINerID),
			loc::t.notif.core.inptverf.pinasciierr0, loc::t.notif.core.inptverf.pinasciierr1);
		return false;
	}
	if (!LwrsFileUnit::isPINreal(PIN)) {
		BeforRetFALSE(ltxt::rf::PinHashErr,
			loc::t.notif.core.inptverf.pinhasherr0, loc::t.notif.core.inptverf.pinhasherr1);
		return false;
	}

	// ---------------------------------
	//		Save
	// --------------------------------------------------------------------------------------------
	bool ret = SafeSaveBy(true, Settings::is_2file, Settings::path_f0, Settings::path_f1, PIN, TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);
	SecureWipe(TEMP_PASS_NFC, Const::MASTER_PASS_SIZE);

	if (ret) {
		NotifService::AddMessage(MType::M_NORMAL,
			loc::t.notif.core.sv.savedone0, loc::t.notif.core.sv.savedone1);
		Exit_PIN_CONFIRM_SAVE();
		is_Save = true;
	}
	else {
		NotifService::AddMessage(MType::M_ERROR,
			loc::t.notif.core.sv.errsave0, loc::t.notif.core.sv.errsave1);
	}
	return ret;
}

bool Core::Try_REMAKE_FILE() {
	// ---------------------------------
	//		Temp
	// --------------------------------------------------------------------------------------------
	constexpr size_t s_UINT = Const::MASTER_PASS_SIZE;
	constexpr size_t s_CHAR = Const::MASTER_PASS_SIZE + 1;

	uint8_t OLDPASS_NFC[s_UINT]			= {}; SecureWipe(OLDPASS_NFC,			s_UINT);
	uint8_t CONFIRM_OLDPASS_NFC[s_UINT] = {}; SecureWipe(CONFIRM_OLDPASS_NFC,	s_UINT);
	uint8_t NEWPASS_NFC[s_UINT]			= {}; SecureWipe(NEWPASS_NFC,			s_UINT);
	uint8_t CONFIRM_NEWPASS_NFC[s_UINT] = {}; SecureWipe(CONFIRM_NEWPASS_NFC,	s_UINT);

	UTF8_to_NFC::to_nfc_bytes(MASTER_PASS,				s_CHAR, OLDPASS_NFC,			s_UINT);
	UTF8_to_NFC::to_nfc_bytes(ENTER_OLD_MASTER_PASS,	s_CHAR, CONFIRM_OLDPASS_NFC,	s_UINT);
	UTF8_to_NFC::to_nfc_bytes(NEW_MASTER_PASS_ENTER,	s_CHAR, NEWPASS_NFC,			s_UINT);
	UTF8_to_NFC::to_nfc_bytes(NEW_MASTER_PASS_CONFIRM,	s_CHAR, CONFIRM_NEWPASS_NFC,	s_UINT);

	auto claerTEMP = [&]() {
		SecureWipe(OLDPASS_NFC, s_UINT);
		SecureWipe(CONFIRM_OLDPASS_NFC, s_UINT);
		SecureWipe(NEWPASS_NFC, s_UINT);
		SecureWipe(CONFIRM_NEWPASS_NFC, s_UINT);
	};

	auto BeforRetFALSE = [&](const std::string& log, const std::string& NotifHeader, const std::string& NotifBoby) {
		Log::add(log, L_ERROR);
		NotifService::AddMessage(MType::M_ERROR, NotifHeader, NotifBoby);

		claerTEMP();
	};

	// ---------------------------------
	//		Check old PIN  &  Check NEW PIN ASCII
	// --------------------------------------------------------------------------------------------
	if (!LwrsFileUnit::isPINreal(PIN)) {
		BeforRetFALSE(ltxt::rf::PinHashErr,
			loc::t.notif.core.inptverf.pinhasherr0, loc::t.notif.core.inptverf.pinhasherr1);
		return false;
	}
	int PINerID = 0;
	if (!isPIN_ASCII(PINerID, NEW_PIN)) {
		BeforRetFALSE(std::format(ltxt::rf::NewPinNonASCII_FT, PINerID),
			loc::t.notif.core.inptverf.newpinasciierr0, loc::t.notif.core.inptverf.newpinasciierr1);
		return false;
	}

	// ---------------------------------
	//		OLD PASS & NEW PASS | CONFIRM
	// --------------------------------------------------------------------------------------------
	int O_P_erID = 0;
	if (!isPASSCONFIRM(O_P_erID, OLDPASS_NFC, s_UINT, CONFIRM_OLDPASS_NFC, s_UINT)) {
		// dont say the id of unmatch !! 
		BeforRetFALSE(ltxt::rf::OldPassInvalid,
			loc::t.notif.core.inptverf.oldpasserr0, loc::t.notif.core.inptverf.oldpasserr1);
		return false;
	}
	int N_P_erID = 0;
	if (!isPASSCONFIRM(N_P_erID, NEWPASS_NFC, s_UINT, CONFIRM_NEWPASS_NFC, s_UINT)) {
		BeforRetFALSE(std::format(ltxt::rf::NewPassNotsame_FT, N_P_erID),
			loc::t.notif.core.inptverf.newpasserr0, loc::t.notif.core.inptverf.newpasserr1);
		return false;
	}

	// ---------------------------------
	//		Save
	// --------------------------------------------------------------------------------------------
	bool ret = SafeSaveBy(true, Settings::is_2file, Settings::path_f0, Settings::path_f1, NEW_PIN, NEWPASS_NFC, s_UINT);

	if (ret) {
		NotifService::AddMessage(MType::M_NORMAL,
			loc::t.notif.core.sv.savedone0, loc::t.notif.core.sv.savedone1);
		EXIT_TO_START(); // already make all clear inside
	}
	else {
		NotifService::AddMessage(MType::M_ERROR,
			loc::t.notif.core.sv.errsave0, loc::t.notif.core.sv.errsave1);
	}

	claerTEMP();
	return ret;
}

bool Core::Try_COPY_FILE() {
	// ---------------------------------
	//		Temp
	// --------------------------------------------------------------------------------------------
	std::string TEMP_path0;
	std::string TEMP_path1;

	constexpr size_t s_UINT = Const::MASTER_PASS_SIZE;
	constexpr size_t s_CHAR = Const::MASTER_PASS_SIZE + 1;

	uint8_t OLDPASS_NFC[s_UINT] = {}; SecureWipe(OLDPASS_NFC, s_UINT);
	uint8_t CONFIRM_OLDPASS_NFC[s_UINT] = {}; SecureWipe(CONFIRM_OLDPASS_NFC, s_UINT);
	uint8_t NEWPASS_NFC[s_UINT] = {}; SecureWipe(NEWPASS_NFC, s_UINT);
	uint8_t CONFIRM_NEWPASS_NFC[s_UINT] = {}; SecureWipe(CONFIRM_NEWPASS_NFC, s_UINT);

	UTF8_to_NFC::to_nfc_bytes(MASTER_PASS, s_CHAR, OLDPASS_NFC, s_UINT);
	UTF8_to_NFC::to_nfc_bytes(ENTER_OLD_MASTER_PASS, s_CHAR, CONFIRM_OLDPASS_NFC, s_UINT);
	UTF8_to_NFC::to_nfc_bytes(NEW_MASTER_PASS_ENTER, s_CHAR, NEWPASS_NFC, s_UINT);
	UTF8_to_NFC::to_nfc_bytes(NEW_MASTER_PASS_CONFIRM, s_CHAR, CONFIRM_NEWPASS_NFC, s_UINT);

	uint8_t* TEMP_GFILE_DATA = nullptr;
	size_t TEMP_GFILE_DATA_SIZE = 0;

	uint8_t* TEMP_D0 = nullptr;
	uint8_t* TEMP_D1 = nullptr;
	size_t TEMP_D0_SIZE = 0;
	size_t TEMP_D1_SIZE = 0;

	auto clearTEMP = [&]() {
		TEMP_path0.clear();		TEMP_path0.shrink_to_fit();
		TEMP_path1.clear();		TEMP_path1.shrink_to_fit();
		
		SecureWipe(OLDPASS_NFC, s_UINT);
		SecureWipe(CONFIRM_OLDPASS_NFC, s_UINT);
		SecureWipe(NEWPASS_NFC, s_UINT);
		SecureWipe(CONFIRM_NEWPASS_NFC, s_UINT);

		SecureWipe(TEMP_GFILE_DATA, TEMP_GFILE_DATA_SIZE);
		delete[] TEMP_GFILE_DATA; TEMP_GFILE_DATA = nullptr;
		TEMP_GFILE_DATA_SIZE = 0;

		SecureWipe(TEMP_D0, TEMP_D0_SIZE);
		SecureWipe(TEMP_D1, TEMP_D1_SIZE);
		delete[] TEMP_D0; TEMP_D0 = nullptr;
		delete[] TEMP_D1; TEMP_D1 = nullptr;
		TEMP_D0_SIZE = 0;
		TEMP_D1_SIZE = 0;
	};

	auto BeforRetFALSE = [&](const std::string& log, const std::string& NotifHeader, const std::string& NotifBoby) {
		Log::add(log, L_ERROR);
		NotifService::AddMessage(MType::M_ERROR, NotifHeader, NotifBoby);

		clearTEMP();
	};

	// ---------------------------------
	//		Path Combine & Check
	// --------------------------------------------------------------------------------------------
	// log & notif inside PathCombName
	if (IS_2_FILE_TEMP) {
		if (!Path2CombName(TEMP_path0, TEMP_path1, NEW_PATH_0, NEW_PATH_1, NEW_NAME)) return false;
	}
	else {
		if (!PathCombName(TEMP_path0, NEW_PATH_0, NEW_NAME)) return false;
	}

	// ---------------------------------
	//		Check old PIN  &  Check NEW PIN ASCII
	// --------------------------------------------------------------------------------------------
	if (!LwrsFileUnit::isPINreal(PIN)) {
		BeforRetFALSE(ltxt::rf::PinHashErr, loc::t.notif.core.inptverf.pinhasherr0, loc::t.notif.core.inptverf.pinhasherr1);
		return false;
	}
	int PINerID = 0;
	if (!isPIN_ASCII(PINerID, NEW_PIN)) {
		BeforRetFALSE(std::format(ltxt::rf::NewPinNonASCII_FT, PINerID),
			loc::t.notif.core.inptverf.newpinasciierr0, loc::t.notif.core.inptverf.newpinasciierr1);
		return false;
	}

	// ---------------------------------
	//		OLD PASS & NEW PASS | CONFIRM
	// --------------------------------------------------------------------------------------------
	int O_P_erID = 0;
	if (!isPASSCONFIRM(O_P_erID, OLDPASS_NFC, s_UINT, CONFIRM_OLDPASS_NFC, s_UINT)) {
		// dont say the id of unmatch !! 
		BeforRetFALSE(ltxt::rf::OldPassInvalid,
			loc::t.notif.core.inptverf.oldpasserr0, loc::t.notif.core.inptverf.oldpasserr1);
		return false;
	}
	int N_P_erID = 0;
	if (!isPASSCONFIRM(N_P_erID, NEWPASS_NFC, s_UINT, CONFIRM_NEWPASS_NFC, s_UINT)) {
		BeforRetFALSE(std::format(ltxt::rf::NewPassNotsame_FT, N_P_erID),
			loc::t.notif.core.inptverf.newpasserr0, loc::t.notif.core.inptverf.newpasserr1);
		return false;
	}

	// ---------------------------------
	//		Save
	// --------------------------------------------------------------------------------------------
	bool ret = SafeSaveBy(false, IS_2_FILE_TEMP, TEMP_path0, TEMP_path1, NEW_PIN, NEWPASS_NFC, s_UINT);

	if (ret) {
		NotifService::AddMessage(MType::M_NORMAL,
			loc::t.notif.core.sv.crtdone0, loc::t.notif.core.sv.crtdone1);
		Exit_COPY_F(); // already make all clear inside
	}
	else {
		NotifService::AddMessage(MType::M_ERROR, loc::t.notif.core.sv.errsave0, loc::t.notif.core.sv.errsave1);
	}

	clearTEMP();
	return ret;
}



//      ___ _  _ ___ _____ 
//     |_ _| \| |_ _|_   _|
//      | || .` || |  | |  
//     |___|_|\_|___| |_|  
// ------------------------------------------------------------------
void Core::_INIT_() {

	{
		// License s 

		if (!FileSys::Ping_FileExists(FileSys::GetLicense_FP_utf8())) {
			if (FileSys::write_text_to_file(FileSys::GetLicense_FP_utf8(), SELF_LICENSE) == FileSys::Status::Success) {
				Log::add(std::format(ltxt::SaveLicenseOK_FT, FileSys::GetLicense_FP_utf8()), L_OK);
			}
		}

		for (const auto& lcs : THIRD_PARTY_LICENSES) {
			if (!FileSys::Ping_FileExists(FileSys::GetLicenses_FP_utf8(lcs.name))) {
				if (FileSys::write_text_to_file(FileSys::GetLicenses_FP_utf8(lcs.name), lcs.text) ==
					FileSys::Status::Success)
				{
					Log::add(std::format(ltxt::SaveLicenseOK_FT, FileSys::GetLicenses_FP_utf8(lcs.name)), L_OK);
				}
			}
		}
	}
	

	Settings::Load();

	ui_context.ui_state = UI_state::Intro;
	ui_context.Width = Settings::Width;
	ui_context.Height = Settings::Height + Const::WIN_HEADER_PX ;

	LocalSys::_INIT_();

	Log::add(ltxt::EndInit, L_INFO);
}

void Core::_UNINIT_() {
	LwrsFileUnit::ClearSelf();
	Clear_ALL();

	// Setting
	if (Settings::is_forget_after_exit) {
		SecureWipe(Settings::path_f0, sizeof(Settings::path_f0));
		SecureWipe(Settings::path_f1, sizeof(Settings::path_f1));
	}
	if (!Settings::is_2file) {
		Settings::ClearFPath1();
	}
	Settings::Save();
}
