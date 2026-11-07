#pragma once
#include <raylib.h>

class DownBar_PN {
public:
	DownBar_PN() = delete;

	// Down Line 
	static void DrawDownBar(struct Rectangle aa);

	// Expand Log
	static void DrawLog(struct Rectangle aa_zone, struct Rectangle aa_db);
	
	// Expand Version
	static void DrawVersion(struct Rectangle aa_zone, struct Rectangle aa_db);
		
	// reset version scroll
	static void Reset();
};


