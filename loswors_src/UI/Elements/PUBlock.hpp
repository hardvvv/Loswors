#pragma once
#include <raylib.h>

enum class PU_Interact_Status {
	NONE,
	POS_WAS_CHANGE,
	ENTER_EDIT,
	DELETE_DONE
};

class PUBlock {
public:
	PUBlock() = delete;

	static PU_Interact_Status Draw(struct Rectangle aa, float outpadPX, size_t PUfloatID, bool isCastTouch, bool is_InZone, Rectangle out_scissor);

};