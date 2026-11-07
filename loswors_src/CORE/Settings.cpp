#include "Settings.hpp"

#include <algorithm>
#include <string>
#include <format>
#include <sstream>

#include "Const.hpp"
#include "Log.hpp"
#include "FS/FileSys.hpp"
#include "../UI/Theme/Theme.hpp"
#include "../Localization/LocalSys.hpp"

namespace {
	namespace ltxt {
		constexpr char OkLoad[]			= "[ Settings ] >> Load self from file";
		constexpr char OkWrite[]		= "[ Settings ] >> Write self to file";

		constexpr char ErrOpen[]		= "[ Settings ] >> Save file -> Error open";
		constexpr char ErrCreateDir[]	= "[ Settings ] >> Save file -> Error create dir";
		constexpr char ErrWrite[]		= "[ Settings ] >> Save file -> Write error";
	}
}

namespace {
	int safe_stoi(const std::string& str) {
		try {
			return std::stoi(str);
		}
		catch (...) {
			return 0;
		}
	}
}



//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

bool Settings::is_2file					= false;
bool Settings::is_forget_after_exit		= false;
bool Settings::is_no_file_exit			= false;
bool Settings::is_show_decode_anim		= true;
bool Settings::is_random_theme			= false;

char Settings::path_f0[Const::MAX_PATH_LEN + 1] = {};
char Settings::path_f1[Const::MAX_PATH_LEN + 1] = {};

int Settings::Width						= Const::WIDTH_DEF;
int Settings::Height					= Const::HEIGHT_DEF;
int Settings::ui_theme_id				= 0;

char Settings::language[Const::MAX_LANGUAGE_NAME + 1] = "en";

int Settings::inactive_timeout_sec = 60;

// ----------- func -----------

void Settings::Load() {
	std::vector<std::string> lines;
	FileSys::Status status = FileSys::read_text_lines_from_file(lines, FileSys::GetSettings_FP_utf8());

	if (status == FileSys::Status::Success) {
		for (int i = 0; i < lines.size(); i++) {

			std::istringstream iss(lines[i]);
			std::string key, value;

			if (!std::getline(iss, key, '='))	continue;
			if (!std::getline(iss, value))		continue;

			if (key == "is_2file") {
				is_2file = safe_stoi(value);
			}
			else if (key == "is_forget_after_exit") {
				is_forget_after_exit = safe_stoi(value); 
			}
			else if (key == "is_no_file_exit") {
				is_no_file_exit = safe_stoi(value);
			}
			else if (key == "is_show_decode_anim") {
				is_show_decode_anim = safe_stoi(value);
			}
			else if (key == "is_random_theme") {
				is_random_theme = safe_stoi(value);
			}
			else if (key == "path_f0") {
				strncpy(path_f0, value.c_str(), sizeof(path_f0) - 1);
				path_f0[sizeof(path_f0) - 1] = '\0';
			}
			else if (key == "path_f1") {
				strncpy(path_f1, value.c_str(), sizeof(path_f1) - 1);
				path_f1[sizeof(path_f1) - 1] = '\0';
			}
			else if (key == "Width") {
				Width = safe_stoi(value);
			}
			else if (key == "Height") {
				Height = safe_stoi(value);
			}
			else if (key == "ui_theme_id") {
				ui_theme_id = safe_stoi(value);
			}
			else if (key == "language") {
				strncpy(language, value.c_str(), sizeof(language) - 1);
				language[sizeof(language) - 1] = '\0';
			}
			else if (key == "inactive_timeout_sec") {
				inactive_timeout_sec = safe_stoi(value);
			}
		}
		// normalize
		Width	= std::clamp(Width, Const::WIDTH_MIN, Const::WIDTH_MAX);
		Height	= std::clamp(Height, Const::HEIGHT_MIN, Const::HEIGHT_MAX);

		// need to clamp UI Theme !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		ui_theme_id = std::clamp(ui_theme_id, 0, Theme::GetThemeCount());

		inactive_timeout_sec = std::clamp(inactive_timeout_sec,
			Const::MIN_INACTIVE_TIMEOUT_s, Const::MAX_INACTIVE_TIMEOUT_s);

		Log::add(ltxt::OkLoad, L_INFO);
	}
	else if (status == FileSys::Status::FailedToOpen) {
		Log::add(ltxt::ErrOpen, L_ERROR);
	}
}

// raw write ! secure write need befor !
void Settings::Save() {
	std::string savetext = std::format(
		"is_2file={}\n"
		"is_forget_after_exit={}\n"
		"is_no_file_exit={}\n"
		"is_show_decode_anim={}\n"
		"is_random_theme={}\n"

		"path_f0={}\n"
		"path_f1={}\n"

		"Width={}\n"
		"Height={}\n"
		"ui_theme_id={}\n"

		"language={}\n"

		"inactive_timeout_sec={}\n",

		std::to_string(is_2file),
		std::to_string(is_forget_after_exit),
		std::to_string(is_no_file_exit),
		std::to_string(is_show_decode_anim),
		std::to_string(is_random_theme),

		path_f0, path_f1,
		
		Width, Height, ui_theme_id,
		
		LocalSys::GetCurLangName(),

		inactive_timeout_sec
	);

	FileSys::Status status = FileSys::write_text_to_file(FileSys::GetSettings_FP_utf8(), savetext);

	if (status == FileSys::Status::Success) {
		Log::add(ltxt::OkWrite, L_OK);
	}
	else if (status == FileSys::Status::DirectoryCreationFailed) {
		Log::add(ltxt::ErrCreateDir, L_ERROR);
	}
	else if (status == FileSys::Status::WriteError) {
		Log::add(ltxt::ErrWrite, L_ERROR);
	}
}


void Settings::ClearFPath0() {
	path_f0[0] = '\0';
}
void Settings::ClearFPath1() {
	path_f1[0] = '\0';
}
void Settings::SetDef() {
	is_2file				= false;
	is_forget_after_exit	= false;
	is_no_file_exit			= false;
	is_show_decode_anim		= true;
	is_random_theme			= false;


	// file path
	ClearFPath0();
	ClearFPath1();

	Width = Const::WIDTH_DEF;
	Height = Const::HEIGHT_DEF;
	ui_theme_id = 0;


	// localization
	language[0] = 'e'; language[1] = 'n'; language[2] = '\0';  // easy for CPU
	//strncpy(language, "en", 3);

	inactive_timeout_sec = 60;
}