#pragma once
#include <string>
#include <vector>

#include "../Resources/Resources.hpp"

#include "UI_Obj.hpp"


enum class MType { M_NORMAL, M_WARNING, M_ERROR };

class NotifService {
public:
	NotifService() = delete;

	static void Draw(Rectangle aa);

	static void AddMessage(MType type, const std::string& name, const std::string& text);

	static void ClearAll();

	static bool isHover();

};