#pragma once
#include <raylib.h>

class PassGen {
public:
	PassGen() = delete;

	static void Reset();
	static void Draw(struct Rectangle aa);

};