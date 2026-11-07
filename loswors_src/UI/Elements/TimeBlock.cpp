#include "TimeBlock.hpp"
#include <raygui.h>

#include <ctime>

#include "../../Resources/Resources.hpp"
#include "../Theme/Theme.hpp"
#include "../UI.hpp"

constexpr float
    pad = 0.05f,
    
    t = 0;

namespace {

    char TIMETEXT[16] = { "00:00:00" };

    bool UpdateTime() {

        std::time_t now = std::time(nullptr);
        struct std::tm timeInfo;

#if defined(_WIN32) || defined(_WIN64)
        if (localtime_s(&timeInfo, &now) != 0) return false;
#else
        if (localtime_r(&now, &timeInfo) == nullptr) return false;
#endif

        std::strftime(TIMETEXT, 16, "%H:%M:%S", &timeInfo);
        return true;
    }
}

void TimeBlock::Draw(Rectangle aa) {
    DrawRectangleRec(aa, { 0, 0, 0, 90 });
    DrawRectangleLinesEx(aa, 1.0f, Theme::C::timeblock_border);

    UpdateTime();

    Vector2 baseSize = MeasureTextEx(Resources::GetFT_main_font(), TIMETEXT, UI::GetTextSize(), 1);

    float scaleX = (aa.width * (1.0f - (pad * 2))) / baseSize.x;
    float scaleY = (aa.height * (1.0f - (pad * 2))) / baseSize.y;

    float finalScale = (scaleX < scaleY) ? scaleX : scaleY;

    float finalFontSize = UI::GetTextSize() * finalScale;

    Vector2 finalSize = { baseSize.x * finalScale, baseSize.y * finalScale };

    Vector2 position = {
        aa.x + (aa.width - finalSize.x) * 0.5f,
        aa.y + (aa.height - finalSize.y) * 0.5f
    };

    DrawTextEx(Resources::GetFT_main_font(), TIMETEXT, position, finalFontSize, finalScale,
        GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
}