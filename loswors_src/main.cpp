#define _CRT_SECURE_NO_WARNINGS  // important to first row (make no error to (fopen sscanf))

#include <vector>
#include <string>
#include <string_view>
#include <format>

// My
#include "CORE/Core.hpp"
#include "CORE/Settings.hpp"
#include "CORE/Log.hpp"
#include "CORE/MonoProg/MonoProg.hpp"
#include "UI/UI.hpp"

#include "CORE/Encode/toUFT8.hpp"


namespace {

	inline void SetArg(int argc, char* argv[]) {
		std::vector<std::string> arguments;

		for (int i = 1; i < argc; i++) {
			arguments.push_back(toUFT8::From(argv[i]));
		}

		if (arguments.size() == 1) {
			std::snprintf(Settings::path_f0, sizeof(Settings::path_f0), "%s", arguments[0].c_str());
			Settings::ClearFPath1();
			Settings::is_2file = false;
		}
		else if (arguments.size() == 2) {
			if (arguments[0].ends_with(std::format("{}{}", "_1.", Const::LOSWORS_FILE_FORMAT)) ||
				arguments[1].ends_with(std::format("{}{}", "_0.", Const::LOSWORS_FILE_FORMAT))) {
				std::swap(arguments[0], arguments[1]);
			}

			std::snprintf(Settings::path_f0, sizeof(Settings::path_f0), "%s", arguments[0].c_str());
			std::snprintf(Settings::path_f1, sizeof(Settings::path_f1), "%s", arguments[1].c_str());
			Settings::is_2file = true;
		}

		if (arguments.size() != 0) {
			Core::END_INTRO();
		}
#ifndef NDEBUG
	Core::END_INTRO(); // delete for build release
#endif NDEBUG

		
	}
}


int main(int argc, char* argv[]) {
	MonoProg::_INIT_();

	Core::_INIT_();
	UI::_INIT_();

	SetArg(argc, argv);

	// ---------- UI ----------

	Log::add("<<", L_INFO);
	Log::add(Const::PROGRAM_NAME, L_ERROR);
	Log::add(Const::LOSWORS_VERSION_TXT, L_OK);
	Log::add(std::format("Code & Design by {}", Const::AUTHOR_NAME), L_WARNING);
	Log::add("- - - - - - - - - - Start Program - - - - - - - - - -", L_INFO);

	UI::StartLoop();

	// ---------- UI ---------- 

	UI::_UNINIT_();
	Core::_UNINIT_();

	return 0;
}