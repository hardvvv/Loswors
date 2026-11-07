#pragma once

#include <vector>
#include <string>

class LocalSys {
public:
	LocalSys() = delete;

	static void LoadLangByID(int id);
	static int GetCurLangID();
	static const char* GetCurLangName();
	static const char* GetAllLangName_forui(); // for ui
	static int GetLangCount();

	static std::vector<int> GetCodePointARR();

	/*
		@brief Load to FS & upload to self all localisation
		Use After Settings Load ! | This function does not recalculate FONT
	*/
	static void _INIT_();
};