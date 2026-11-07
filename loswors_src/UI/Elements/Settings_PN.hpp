#pragma once
#include <raylib.h>

class Settings_PN {
public:
	Settings_PN() = delete;

	static Rectangle GetBtnZone(struct Rectangle aa);
	static void DrawBtnEnter(struct Rectangle aa);

	static void Draw(struct Rectangle aa);

};