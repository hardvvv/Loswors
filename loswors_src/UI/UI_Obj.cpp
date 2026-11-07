#include "UI_Obj.hpp"

#include <raygui.h>

#include <algorithm>
#include <string>
#include <cmath>

#include "../CORE/Const.hpp"
#include "../Resources/Resources.hpp"

#include "Theme/Theme.hpp"

#include "../CORE/FS/FileViewer.hpp"
#include "../Localization/loc.hpp"
#include "../CORE/FS/FileSys.hpp"

namespace {

    inline size_t GetUnitCount(char* text, int max_size) {
        if (text == nullptr || text[0] == '\0') return 0;

        size_t count = 0;
        int bytePos = 0;
        int codepointSize = 0;

        // Iterate through bytes until we encounter a null terminator or reach the max size
        while (text[bytePos] != '\0' && bytePos < max_size) {
            // The function takes the current byte, determines how many bytes it occupies as a character,
            // and returns its size in codepointSize.
            GetCodepointNext(&text[bytePos], &codepointSize);

            bytePos += codepointSize; // Move the position forward by the size of the character in bytes
            count++;                  // Count this as one "unit" (character)
        }

        return count;
    }

    // for InputMultiLine
    struct TextLineInfo {
        size_t startByte;
        size_t byteLen;
        int charCount;
    };
    static std::vector<TextLineInfo> ParseTextLines(const char* text) {
        std::vector<TextLineInfo> lines;
        size_t len = strlen(text);
        size_t start = 0;
        size_t currentByte = 0;
        int chars = 0;

        while (currentByte < len) {
            if (text[currentByte] == '\n') {
                lines.push_back({ start, currentByte - start, chars });
                start = currentByte + 1;
                chars = 0;
                currentByte++;
                continue;
            }

            int bytesRead = 0;
            GetCodepointNext(&text[currentByte], &bytesRead);
            currentByte += bytesRead;
            chars++;
        }

        lines.push_back({ start, currentByte - start, chars });
        return lines;
    }
    static size_t GetByteOffsetInLine(const char* text, size_t startByte, int targetCol) {
        size_t currentByte = startByte;
        int col = 0;
        while (col < targetCol && text[currentByte] != '\0' && text[currentByte] != '\n') {
            int bytesRead = 0;
            GetCodepointNext(&text[currentByte], &bytesRead);
            currentByte += bytesRead;
            col++;
        }
        return currentByte;
    }
    static bool ProcessKeyRepeat(KeyboardKey key, float& timer, float delay = 0.4f, float rate = 0.035f) {
        if (IsKeyPressed(key)) {
            timer = 0.0f;
            return true;
        }
        if (IsKeyDown(key)) {
            timer += GetFrameTime();
            if (timer >= delay) {
                timer -= rate;
                return true;
            }
        }
        if (IsKeyReleased(key)) {
            timer = 0.0f;
        }
        return false;
    }

}

namespace UI_Obj {

    bool isPressInZ(Rectangle zone, bool& pp) {
        bool isCOLIDE = CheckCollisionPointRec(GetMousePosition(), zone);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            pp = isCOLIDE;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            bool isOK = pp && isCOLIDE;

            pp = false;
            return isOK;
        }

        return false;
    }

    void SetWH_minAR(float minAR, Rectangle& zone) {
        if (minAR <= 0) return;

        if (zone.height > 0.0f) {
            if ((zone.width / zone.height) < minAR) {
                float centerY = zone.y + zone.height * 0.5f;
                zone.height = std::round(zone.width / minAR);
                zone.y = std::round(centerY - (zone.height * 0.5f));
            }
        }
    }
//      _____        _                
//     |_   _|____ _| |_ _  _ _ _ ___ 
//       | |/ -_) \ /  _| || | '_/ -_)
//       |_|\___/_\_\\__|\_,_|_| \___|
// ---------------------------------------------------------------------
    void DrawTexture_Color(Rectangle aa, Texture2D texture, Color overcolor) {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        DrawTexturePro(texture, sourceRec, aa, { 0, 0 }, 0.0f, overcolor);
    }
    void DrawTexture_ColorRot(Rectangle aa, Texture2D texture, Color overcolor, float rot) {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };

        Vector2 origin = { aa.width / 2.0f, aa.height / 2.0f };
        Rectangle destBounds = {
            aa.x + origin.x,
            aa.y + origin.y,
            aa.width,
            aa.height
        };
        DrawTexturePro(texture, sourceRec, destBounds, origin, rot, overcolor);
    }

    void DrawTextCenter(Font font, const char* text, Rectangle zone, float fontSize, float spacing, Color color) {
        if (!text || text[0] == '\0') return;

        Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);

        Vector2 textPos = {
            zone.x + (zone.width - textSize.x) * 0.5f,
            zone.y + (zone.height - textSize.y) * 0.5f
        };

        DrawTextEx(font, text, textPos, fontSize, spacing, color);
    }
    void DrawText_FitCenter(Rectangle bounds, const char* text, Font font, Color color, float padding) {
        if (!text || text[0] == '\0') return;
        constexpr float spacing = 1.0f;

        // 1. Calculate padding
        float clampedFactor = std::clamp(padding, 0.0f, 1.0f);
        float minSide = std::min(bounds.width, bounds.height);
        float paddingPixels = (minSide * clampedFactor) * 0.5f;

        float availW = std::max(1.0f, bounds.width - (paddingPixels * 2.0f) - 4.0f);
        float availH = std::max(1.0f, bounds.height - (paddingPixels * 2.0f));

        float maxFontSize = availH;

        // 2. First width measurement
        Vector2 textSize = MeasureTextEx(font, text, maxFontSize, spacing);

        // 3. Proportional calculation of font size
        float finalFontSize = maxFontSize;
        if (textSize.x > availW && textSize.x > 0.0f) {
            finalFontSize = maxFontSize * (availW / textSize.x);
        }

        // 4. Correcting cycle for protection against flight
        Vector2 renderTextSize = MeasureTextEx(font, text, finalFontSize, spacing);
        while (renderTextSize.x > availW && finalFontSize > 6.0f) {
            finalFontSize -= 0.5f;
            renderTextSize = MeasureTextEx(font, text, finalFontSize, spacing);
        }

        // 5. Center positioning
        Vector2 textPos = {
            std::round(bounds.x + (bounds.width - renderTextSize.x) * 0.5f),
            std::round(bounds.y + (bounds.height - renderTextSize.y) * 0.5f)
        };

        // 6. Immediate rendering
        DrawTextEx(font, text, textPos, finalFontSize, spacing, color);
    }

    //      ___ _____ _  _ 
    //     | _ )_   _| \| |
    //     | _ \ | | | .` |
    //     |___/ |_| |_|\_|
    // ---------------------------------------------------------------------
    bool BTN_Image(Rectangle aa, Texture2D texture, Color overcolor, float padding_wh) {
        bool pressed = false;
        Vector2 mousePoint = GetMousePosition();
        bool mouseOver = CheckCollisionPointRec(mousePoint, aa);

        Color finalTint = overcolor;

        if (mouseOver) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                finalTint = { (unsigned char)(overcolor.r * 0.7), (unsigned char)(overcolor.g * 0.7), (unsigned char)(overcolor.b * 0.7), overcolor.a };
            }
            else {
                finalTint = { (unsigned char)fmin(overcolor.r * 1.2, 255), (unsigned char)fmin(overcolor.g * 1.2, 255), (unsigned char)fmin(overcolor.b * 1.2, 255), overcolor.a };
            }
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
        }

        GuiButton(aa, NULL);

        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };

        float btnX = std::floor(aa.x);
        float btnY = std::floor(aa.y);
        float btnW = std::floor(aa.width);
        float btnH = std::floor(aa.height);

        float padVal = std::floor(std::min(btnW, btnH) * padding_wh);
        float destW = btnW - (padVal * 2.0f);
        float destH = btnH - (padVal * 2.0f);

        float destX = btnX + std::floor((btnW - destW) * 0.5f);
        float destY = btnY + std::floor((btnH - destH) * 0.5f);

        Rectangle destRec = { destX, destY, destW, destH };

        DrawTexturePro(texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, finalTint);

        return pressed;
    }
    bool BTN_Image_TX_SQRT_CENTER(Rectangle aa, Texture2D texture, Color overcolor, float padding) {
        bool pressed = false;
        Vector2 mousePoint = GetMousePosition();
        bool mouseOver = CheckCollisionPointRec(mousePoint, aa);

        Color finalTint = overcolor;

        if (mouseOver) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                finalTint = {
                    (unsigned char)(overcolor.r * 0.7f),
                    (unsigned char)(overcolor.g * 0.7f),
                    (unsigned char)(overcolor.b * 0.7f),
                    overcolor.a
                };
            }
            else {
                finalTint = {
                    (unsigned char)fminf(overcolor.r * 1.2f, 255.0f),
                    (unsigned char)fminf(overcolor.g * 1.2f, 255.0f),
                    (unsigned char)fminf(overcolor.b * 1.2f, 255.0f),
                    overcolor.a
                };
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) pressed = true;
        }

        // Draw the base RayGui button
        GuiButton(aa, NULL);

        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };

        // Clamp the padding factor to the range [0.0, 1.0] to avoid negative sizes
        float clampedFactor = std::clamp(padding, 0.0f, 1.0f);

        // Minimum side of the available rectangle
        float baseSide = std::min(aa.width, aa.height);

        // Calculate the size of the side considering the percentage margin
        // When factor = 0.15 (15%), the size becomes 30% smaller (15% on each side)
        float side = baseSide * (1.0f - clampedFactor);

        // Center the reduced square texture inside the Rectangle aa
        Rectangle destRec = {
            aa.x + (aa.width - side) * 0.5f,
            aa.y + (aa.height - side) * 0.5f,
            side,
            side
        };

        DrawTexturePro(texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, finalTint);

        return pressed;
    }

    bool Hide_BTN_Texture(Rectangle aa, Texture2D texture, Color overcolor) {
        Vector2 mousePoint = GetMousePosition();

        bool isHovered = CheckCollisionPointRec(mousePoint, aa);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }

        const int lightoffset = 40;

        Color drawColor = overcolor;

        if (isHovered) {
            drawColor.r = static_cast<unsigned char>(std::min(255, static_cast<int>(overcolor.r) + lightoffset));
            drawColor.g = static_cast<unsigned char>(std::min(255, static_cast<int>(overcolor.g) + lightoffset));
            drawColor.b = static_cast<unsigned char>(std::min(255, static_cast<int>(overcolor.b) + lightoffset));
        }

        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        DrawTexturePro(texture, sourceRec, aa, { 0, 0 }, 0.0f, drawColor);

        return false;
    }

    /*bool Hide_BTN_Texture180(Rectangle aa, Texture2D texture, Color overcolor) {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, aa) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };

        Vector2 origin = { aa.width / 2.0f, aa.height / 2.0f };
        Rectangle destRec = { aa.x + origin.x, aa.y + origin.y, aa.width, aa.height };

        DrawTexturePro(texture, sourceRec, destRec, origin, 180.0f, overcolor);
        return false;
    }*/


    bool BTN_Expand_By_Text(Vector2 outPV, Vector2 inPV,
        const char* text, Font font, float btnH, float spacing, Rectangle* Real_z)
    {
        if (text == nullptr) return false;

        constexpr float TEXT_SCALE_FACTOR = 0.8f;
        float textH = std::round(btnH * TEXT_SCALE_FACTOR);

        Vector2 textSize = MeasureTextEx(font, text, textH, spacing);

        float padINS = std::round((btnH - textH) * 0.5f);
        float btnW = textSize.x + (padINS * 4);
        float posX = std::round(outPV.x - (btnW * inPV.x));
        float posY = std::round(outPV.y - (btnH * inPV.y));

        Rectangle bounds = { posX, posY, btnW, btnH };

        bool isPressed = GuiButton(bounds, NULL);

        Vector2 textPos = { bounds.x + padINS * 2, bounds.y + padINS };

        if (Real_z != nullptr) {
            *Real_z = bounds;
        }

        int state = GuiGetState();
        Color textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL));

        if (state == STATE_FOCUSED) {
            textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_FOCUSED));
        }
        else if (state == STATE_PRESSED) {
            textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_PRESSED));
        }
        else if (state == STATE_DISABLED) {
            textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_DISABLED));
        }

        DrawTextEx(font, text, textPos, textH, spacing, textColor);

        return isPressed;
    }
    bool BTN_Text_FitCenter(Rectangle bounds, const char* text, Font font, float padding) {
        bool result = GuiButton(bounds, NULL);

        // Text color is taken from the standard RayGui button style
        Color textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL));

        // Call an independent drawing function
        DrawText_FitCenter(bounds, text, font, textColor, padding);

        return result;
    }

    void BTN_VizEye(Rectangle aa, Color overcolor, bool& isHide) {
        if (BTN_Image(aa, isHide ? Resources::GetTX_eye_close() : Resources::GetTX_eye_open(), overcolor, 0.1f)) {
            isHide = !isHide;
        }
    }
    void BTN_VizEye_Slider(Vector2 p_out, Vector2 p_in, float side, bool& isHide) {
        Vector2 sz = { std::round(side * 2), std::round(side) };

        Rectangle pos = {
            std::round(p_out.x - (p_in.x * sz.x)),
            std::round(p_out.y - (p_in.y * sz.y)),
            sz.x,
            sz.y
        };

        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        int st = isHide ? 0 : 1;
        GuiToggleSlider(pos, "#45#;#44#", &st);
        if ((isHide && st) || (!isHide && !st)) {
            isHide = !isHide;
        }
    }
    bool BTN_X16(float pivotX, float pivotY, int p0123ul) {
        // 0 -> up left
        Rectangle rt = { pivotX + 16, pivotY, 16, 16 };
        if (p0123ul == 1) rt = { pivotX - 16, pivotY, 16, 16 };
        else if (p0123ul == 2) rt = { pivotX - 16, pivotY - 16, 16, 16 };
        else if (p0123ul == 3) rt = { pivotX, pivotY - 16, 16, 16 };
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        return GuiButton(rt, "#113#");
    }

    //      ___                _   
    //     |_ _|_ _  _ __ _  _| |_ 
    //      | || ' \| '_ \ || |  _|
    //     |___|_||_| .__/\_,_|\__|
    // ------------ |_| ----------------------------------------------------
    void Secure_Input(Rectangle aa, char* text, int maxSize, bool& is_active, bool is_hide) {
        /*
            When the text is larger than 256, when scrolling < to the end,
            there is an overflow, which is fixed by re-entering the text field...
            I don't want to fix this, it's an extremely rare case, so I don't care.
        */

        GuiSetFont(Resources::GetFT_main_font_mono());

        if (is_hide) {
            Color old_dis = GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_DISABLED));
            Color old_focus = GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_FOCUSED));
            Color old_norm = GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_NORMAL));
            Color old_press = GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_PRESSED));

            GuiSetStyle(TEXTBOX, TEXT_COLOR_DISABLED, ColorToInt({ 0,0,0,0 }));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt({ 0,0,0,0 }));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt({ 0,0,0,0 }));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt({ 0,0,0,0 }));

            if (GuiTextBox(aa, text, maxSize, is_active)) {
                is_active = !is_active;
            }

            GuiSetStyle(TEXTBOX, TEXT_COLOR_DISABLED, ColorToInt(old_dis));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(old_focus));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(old_norm));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(old_press));


            size_t len = GetUnitCount(text, maxSize);

            char thide[Const::MASTER_PASS_SIZE + 1] = { 0 };
            if (len > Const::MASTER_PASS_SIZE) len = Const::MASTER_PASS_SIZE;
            memset(thide, '*', len);
            thide[len] = '\0';


            int oldBaseNormal = GuiGetStyle(TEXTBOX, BASE_COLOR_NORMAL);
            int oldBaseFocused = GuiGetStyle(TEXTBOX, BASE_COLOR_FOCUSED);
            int oldBasePressed = GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED);

            int oldBorderNormal = GuiGetStyle(TEXTBOX, BORDER_COLOR_NORMAL);
            int oldBorderFocused = GuiGetStyle(TEXTBOX, BORDER_COLOR_FOCUSED);
            int oldBorderPressed = GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED);


            int alpha = ColorToInt({ 0,0,0,0 });

            GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, alpha);
            GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, alpha);
            GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, alpha);

            GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, alpha);
            GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, alpha);
            GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, alpha);

            GuiLock();
            GuiTextBox(aa, thide, maxSize, false);
            GuiUnlock();

            GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, oldBaseNormal);
            GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, oldBaseFocused);
            GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, oldBasePressed);

            GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, oldBorderNormal);
            GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, oldBorderFocused);
            GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, oldBorderPressed);
        }
        else {
            if (GuiTextBox(aa, text, maxSize, is_active)) {
                is_active = !is_active;
            }
        }
        GuiSetFont(Resources::GetFT_main_font());
    }

    bool InputInt(Rectangle aa, int* value, char* textBuffer, int bufferSize,
        bool* editMode, int min, int max) {
        bool pressed = false;

        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

        if (!(*editMode)) {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            snprintf(textBuffer, bufferSize, "%i", *value);
        }
        GuiSetStyle(DEFAULT, TEXT_SIZE, (int)std::round(aa.height * 0.9f));

        if (GuiTextBox(aa, textBuffer, bufferSize, *editMode)) {
            *editMode = !(*editMode);

            if (!(*editMode)) {                       // redact end
                char* end = nullptr;
                long v = strtol(textBuffer, &end, 10);
                if (end != textBuffer) {              // if redact
                    if (v < min) v = min;
                    if (v > max) v = max;
                    if ((int)v != *value) {           // real change
                        *value = (int)v;
                        pressed = true;
                    }
                }
            }
        }
        return pressed;
    }

    // activeID = -1 if no one selected
    void InputPIN(Rectangle aa, int& activeID, uint8_t(&PIN)[4], bool is_hide) {
        // theme
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);

        const float WpinOne = std::round(aa.width * 0.25f);
        const int INIT_ACT_ID = activeID;

        const float TextWcoef = 0.85f;
        GuiSetStyle(DEFAULT, TEXT_SIZE, (WpinOne * TextWcoef));

        // FLAGs
        int clicked_index = -1;
        int changed_index = -1;
        char new_char = '\0';
        bool lost_focus = false;

        // Draw & get input
        for (int i = 0; i < 4; i++) {
            const bool CONST_isACT = (INIT_ACT_ID == i);
            bool DINAM_isACT = CONST_isACT;

            // if active - empty to draw
            char cast_PIN = CONST_isACT ? '\0' : static_cast<char>(PIN[i]);
            char tochange[2] = { cast_PIN, '\0' };

            UI_Obj::Secure_Input({
                std::floor(aa.x + (WpinOne * i)),
                aa.y,
                WpinOne,
                aa.height
                }, tochange, 2, DINAM_isACT, is_hide);

            // click
            if (DINAM_isACT && !CONST_isACT) {
                clicked_index = i;
            }
            // focus lost
            else if (!DINAM_isACT && CONST_isACT) {
                lost_focus = true;
            }

            // new char enter
            if (tochange[0] != cast_PIN) {
                changed_index = i;
                new_char = tochange[0];
            }
        }

        // force focus lost
        if (clicked_index != -1) {
            activeID = clicked_index;
        }
        else if (lost_focus && activeID == INIT_ACT_ID) {
            activeID = -1;
        }

        // write data
        if (changed_index != -1) {
            PIN[changed_index] = static_cast<uint8_t>(new_char);

            if (new_char != '\0') {
                if (changed_index + 1 < 4) {
                    activeID = changed_index + 1;
                }
                else {
                    activeID = -1; // if last PIN val
                }
            }
        }

        // shift active id 
        if (activeID != -1) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_BACKSPACE)) {
                if (activeID > 0) activeID--;
            }
            else if (IsKeyPressed(KEY_RIGHT)) {
                if (activeID < 3) activeID++;
            }
        }
    }

    void InputMultiLine(
        Rectangle bounds, char* text, size_t maxTextSize,
        int& cursorLine, int& cursorCol, bool readOnly, bool& isActive,
        Vector2& scrollOffset, float fontSize, bool isScroll, Rectangle parentScissor
    ) {
        constexpr float LINE_SPACING_FACTOR = 1.2f;
        constexpr float PADDING_FACTOR = 0.3f;
        constexpr int TAB_SPACES = 4;

        Vector2 mousePoint = GetMousePosition();
        bool mouseOver = CheckCollisionPointRec(mousePoint, bounds);

        bool userInteractedWithCursor = false;

        // 1. Activation / Deactivation by clicking
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            isActive = mouseOver;
        }

        // 2. Drawing the background and border
        Color bgColor = GetColor(GuiGetStyle(TEXTBOX, isActive ? BASE_COLOR_PRESSED : BASE_COLOR_NORMAL));
        Color borderColor = GetColor(GuiGetStyle(TEXTBOX, isActive ? BORDER_COLOR_FOCUSED : BORDER_COLOR_NORMAL));

        float lineW = 1.0f;
        DrawRectangleRec(bounds, bgColor);
        DrawRectangleLinesEx(bounds, lineW, borderColor);

        bounds.x += lineW;
        bounds.y += lineW;
        bounds.width -= lineW * 2;
        bounds.height -= lineW * 2;

        // NEED TO USE MONO !
        Font font = Resources::GetFT_main_font_mono();
        float charWidth = MeasureTextEx(font, "A", fontSize, 0.0f).x;
        float lineHeight = std::round(fontSize * LINE_SPACING_FACTOR);
        float padding = std::round(fontSize * PADDING_FACTOR);

        Rectangle innerBounds = {
            bounds.x + padding,
            bounds.y + padding,
            bounds.width - padding * 2.0f,
            bounds.height - padding * 2.0f
        };

        if (innerBounds.width <= 0 || innerBounds.height <= 0) return;

        std::vector<TextLineInfo> lines = ParseTextLines(text);

        cursorLine = std::clamp(cursorLine, 0, (int)lines.size() - 1);
        cursorCol = std::clamp(cursorCol, 0, lines[cursorLine].charCount);

        // 4. Input and editing text
        if (isActive && !readOnly) {

            // --- Input symbols (UTF-8) ---
            int codepoint = GetCharPressed();
            while (codepoint > 0) {
                if ((codepoint >= 32) || codepoint == '\n') {
                    int codepointByteSize = 0;
                    const char* utf8Char = CodepointToUTF8(codepoint, &codepointByteSize);
                    size_t totalBytes = strlen(text);

                    if (totalBytes + codepointByteSize < maxTextSize) {
                        size_t insertPos = GetByteOffsetInLine(text, lines[cursorLine].startByte, cursorCol);

                        memmove(&text[insertPos + codepointByteSize], &text[insertPos], totalBytes - insertPos + 1);
                        memcpy(&text[insertPos], utf8Char, codepointByteSize);

                        lines = ParseTextLines(text);

                        if (codepoint == '\n') {
                            cursorLine++;
                            cursorCol = 0;
                        }
                        else {
                            cursorCol++;
                        }
                        userInteractedWithCursor = true;
                    }
                }
                codepoint = GetCharPressed();
            }

            // --- Enter ---
            if (IsKeyPressed(KEY_ENTER)) {
                size_t totalBytes = strlen(text);
                if (totalBytes + 1 < maxTextSize) {
                    size_t insertPos = GetByteOffsetInLine(text, lines[cursorLine].startByte, cursorCol);

                    memmove(&text[insertPos + 1], &text[insertPos], totalBytes - insertPos + 1);
                    text[insertPos] = '\n';

                    lines = ParseTextLines(text);
                    cursorLine++;
                    cursorCol = 0;
                    userInteractedWithCursor = true;
                }
            }

            // --- Tab ---
            if (IsKeyPressed(KEY_TAB)) {
                size_t totalBytes = strlen(text);
                if (totalBytes + TAB_SPACES < maxTextSize) {
                    size_t insertPos = GetByteOffsetInLine(text, lines[cursorLine].startByte, cursorCol);

                    memmove(&text[insertPos + TAB_SPACES], &text[insertPos], totalBytes - insertPos + 1);
                    memset(&text[insertPos], ' ', TAB_SPACES);

                    lines = ParseTextLines(text);
                    cursorCol += TAB_SPACES;
                    userInteractedWithCursor = true;
                }
            }

            // --- Backspace with automatic repetition ---
            static float backspaceTimer = 0.0f;
            if (ProcessKeyRepeat(KEY_BACKSPACE, backspaceTimer)) {
                size_t totalBytes = strlen(text);

                if (cursorCol > 0) {
                    size_t deleteEnd = GetByteOffsetInLine(text, lines[cursorLine].startByte, cursorCol);
                    size_t deleteStart = GetByteOffsetInLine(text, lines[cursorLine].startByte, cursorCol - 1);

                    memmove(&text[deleteStart], &text[deleteEnd], totalBytes - deleteEnd + 1);
                    lines = ParseTextLines(text);
                    cursorCol--;
                    userInteractedWithCursor = true;
                }
                else if (cursorLine > 0) {
                    int prevLine = cursorLine - 1;
                    int prevCol = lines[prevLine].charCount;
                    size_t deletePos = lines[cursorLine].startByte - 1;

                    memmove(&text[deletePos], &text[deletePos + 1], totalBytes - deletePos);
                    lines = ParseTextLines(text);

                    cursorLine = prevLine;
                    cursorCol = prevCol;
                    userInteractedWithCursor = true;
                }
            }

            // --- Arrow keys with autorepeat ---
            static float arrowLeftTimer = 0.0f;
            static float arrowRightTimer = 0.0f;
            static float arrowUpTimer = 0.0f;
            static float arrowDownTimer = 0.0f;

            if (ProcessKeyRepeat(KEY_LEFT, arrowLeftTimer)) {
                if (cursorCol > 0) {
                    cursorCol--;
                }
                else if (cursorLine > 0) {
                    cursorLine--;
                    cursorCol = lines[cursorLine].charCount;
                }
                userInteractedWithCursor = true;
            }
            if (ProcessKeyRepeat(KEY_RIGHT, arrowRightTimer)) {
                if (cursorCol < lines[cursorLine].charCount) {
                    cursorCol++;
                }
                else if (cursorLine < (int)lines.size() - 1) {
                    cursorLine++;
                    cursorCol = 0;
                }
                userInteractedWithCursor = true;
            }
            if (ProcessKeyRepeat(KEY_UP, arrowUpTimer)) {
                if (cursorLine > 0) {
                    cursorLine--;
                    cursorCol = std::min(cursorCol, lines[cursorLine].charCount);
                }
                userInteractedWithCursor = true;
            }
            if (ProcessKeyRepeat(KEY_DOWN, arrowDownTimer)) {
                if (cursorLine < (int)lines.size() - 1) {
                    cursorLine++;
                    cursorCol = std::min(cursorCol, lines[cursorLine].charCount);
                }
                userInteractedWithCursor = true;
            }
        }

        // 6. Calculate content area size
        int maxLineChars = 1;
        for (const auto& line : lines) {
            if (line.charCount > maxLineChars) maxLineChars = line.charCount;
        }

        Rectangle contentBounds = {
            0, 0,
            (float)(maxLineChars * charWidth) + padding * 2.0f,
            (float)(lines.size() * lineHeight) + padding * 2.0f
        };

        // 7. Calculate the visible area considering the width of scrollbars
        int scrollbarWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

        bool hasScrollV = contentBounds.height > innerBounds.height;
        bool hasScrollH = contentBounds.width > innerBounds.width;

        Rectangle visibleBounds = innerBounds;
        if (hasScrollV) visibleBounds.width -= scrollbarWidth;
        if (hasScrollH) visibleBounds.height -= scrollbarWidth;

        // 5. Mouse click on text
        if (isActive && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            (mouseOver && CheckCollisionPointRec(mousePoint, visibleBounds))) {
            float relX = mousePoint.x - innerBounds.x - scrollOffset.x;
            float relY = mousePoint.y - innerBounds.y - scrollOffset.y;

            cursorLine = std::clamp((int)(relY / lineHeight), 0, (int)lines.size() - 1);
            cursorCol = std::clamp((int)(std::round(relX / charWidth)), 0, lines[cursorLine].charCount);
            userInteractedWithCursor = true;
        }

        // 8. Auto-scrolling
        if (userInteractedWithCursor) {
            float cursorX = cursorCol * charWidth;
            float cursorY = cursorLine * lineHeight;

            if (cursorX + scrollOffset.x < 0) {
                scrollOffset.x = -cursorX;
            }
            else if (cursorX + charWidth + scrollOffset.x > visibleBounds.width) {
                scrollOffset.x = visibleBounds.width - (cursorX + charWidth);
            }

            if (cursorY + scrollOffset.y < 0) {
                scrollOffset.y = -cursorY;
            }
            else if (cursorY + lineHeight + scrollOffset.y > visibleBounds.height) {
                scrollOffset.y = visibleBounds.height - (cursorY + lineHeight);
            }
        }

        float minScrollX = std::min(0.0f, visibleBounds.width - contentBounds.width);
        float minScrollY = std::min(0.0f, visibleBounds.height - contentBounds.height);
        scrollOffset.x = std::clamp(scrollOffset.x, minScrollX, 0.0f);
        scrollOffset.y = std::clamp(scrollOffset.y, minScrollY, 0.0f);

        // 9. Drawing of GuiScrollPanel and precise scissor
        Vector2 prevScroll = scrollOffset;

        Rectangle viewBounds = { 0 };
        GuiScrollPanel(bounds, NULL, contentBounds, &scrollOffset, &viewBounds);

        if (!isScroll && mouseOver && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            scrollOffset.y = prevScroll.y;
            scrollOffset.x = prevScroll.x;
        }

        Rectangle myScissor = {
        (float)(int)visibleBounds.x - padding,
        (float)(int)visibleBounds.y - padding,
        (float)(int)(visibleBounds.width + padding * 2),
        (float)(int)(visibleBounds.height + padding * 2)
        };

        // 2. If an external rectangle (not null) is provided, intersect it with our
        if (parentScissor.width > 0 && parentScissor.height > 0) {
            float x1 = std::max(myScissor.x, parentScissor.x);
            float y1 = std::max(myScissor.y, parentScissor.y);
            float x2 = std::min(myScissor.x + myScissor.width, parentScissor.x + parentScissor.width);
            float y2 = std::min(myScissor.y + myScissor.height, parentScissor.y + parentScissor.height);

            myScissor.x = x1;
            myScissor.y = y1;
            myScissor.width = std::max(0.0f, x2 - x1);
            myScissor.height = std::max(0.0f, y2 - y1);
        }
        BeginScissorMode((int)myScissor.x, (int)myScissor.y, (int)myScissor.width, (int)myScissor.height);

        Color textColor = GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_NORMAL));

        // Draw symbols
        for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
            const auto& lineInfo = lines[lineIdx];
            size_t currentByte = lineInfo.startByte;
            int col = 0;

            while (currentByte < lineInfo.startByte + lineInfo.byteLen) {
                int bytesRead = 0;
                int cp = GetCodepointNext(&text[currentByte], &bytesRead);

                Vector2 pos = {
                    visibleBounds.x + (col * charWidth) + scrollOffset.x,
                    visibleBounds.y + (lineIdx * lineHeight) + scrollOffset.y
                };

                if (pos.y + lineHeight >= visibleBounds.y && pos.y <= visibleBounds.y + visibleBounds.height &&
                    pos.x + charWidth >= visibleBounds.x && pos.x <= visibleBounds.x + visibleBounds.width) {
                    DrawTextCodepoint(font, cp, pos, fontSize, textColor);
                }

                currentByte += bytesRead;
                col++;
            }
        }

        // 10. Draw cursor
        if (isActive && !readOnly) {
            if ((int)(GetTime() * 2) % 2 == 0) {
                Vector2 caretPos = {
                    visibleBounds.x + (cursorCol * charWidth) + scrollOffset.x,
                    visibleBounds.y + (cursorLine * lineHeight) + scrollOffset.y
                };

                if (caretPos.y >= visibleBounds.y && caretPos.y + lineHeight <= visibleBounds.y + visibleBounds.height &&
                    caretPos.x >= visibleBounds.x && caretPos.x <= visibleBounds.x + visibleBounds.width) {

                    Rectangle caretRect = { caretPos.x, caretPos.y, 2.0f, lineHeight };
                    DrawRectangleRec(caretRect, GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_FOCUSED)));
                }
            }
        }
        EndScissorMode();
        if (parentScissor.width > 0 && parentScissor.height > 0) {
            BeginScissorMode((int)parentScissor.x, (int)parentScissor.y, (int)parentScissor.width, (int)parentScissor.height);
        }
    }

    void PATH_DIR(Rectangle aa, char* path, size_t path_len, bool& is_active, void (*clcPATHf)(), int fileID) {

        constexpr float
            path_y0 = 0.25f,
            pad_refH = 0.12f;

        float y0path = std::round(aa.height * path_y0);
        float Hpath = aa.height - y0path;
        float pad = std::round(aa.height * pad_refH);

        Rectangle fBTNz = { aa.x, aa.y, aa.height, aa.height };
        Rectangle PATHz = { fBTNz.x + fBTNz.width + pad, aa.y + y0path, aa.width - fBTNz.width - Hpath - pad * 2, Hpath };
        Rectangle GALCROSz = { PATHz.x + PATHz.width + pad, PATHz.y, Hpath, Hpath };

        // draw //--//--//--//--//--//--//--//--//

        Color Fcolor = Theme::C::dirview_file;

        if (GuiTextBox(PATHz, path, path_len, is_active)) is_active = !is_active;
        if (UI_Obj::BTN_X16(PATHz.x, PATHz.y, 3) && clcPATHf != nullptr) clcPATHf();

        if (UI_Obj::Hide_BTN_Texture(fBTNz, Resources::GetTX_file_check(), Fcolor)) {
            FileViewer::OpenFolder(path, path_len, loc::t.pathview.selectdir);
        }

        char v[2] = { fileID == 0 ? '0' : '1', '\0' };
        DrawTextEx(Resources::GetFT_main_font(), v,
            { fBTNz.x + fBTNz.width * 0.665f, fBTNz.y + fBTNz.height * 0.46f }, fBTNz.height * 0.4f, 1, Fcolor);

        bool if_ex0 = FileSys::Ping_DirExists(path);
        UI_Obj::DrawTexture_Color(
            GALCROSz, if_ex0 ? Resources::GetTX_ok_gal() : Resources::GetTX_cros(), if_ex0 ? GREEN : RED);

    }
    
    bool SliderInt_W(Rectangle bounds, int* value, int minVal, int maxVal, int step, Font font) {
        if (value == nullptr || minVal >= maxVal || step <= 0) return false;

        // ID slider to prevent capturing of other elements
        static void* activeSliderId = nullptr;
        void* currentSliderId = reinterpret_cast<void*>(value);

        Vector2 mousePos = GetMousePosition();
        bool isLeftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool isLeftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool isLeftReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        // 1. ADAPTIVE AND SAFE SCROLLBAR SIZE
        // Scrollbar height is proportional to the track, but is limited to reasonable bounds (from 16px to 32px)
        float knobH = std::clamp(bounds.height * 0.10f, 18.0f, 32.0f);
        knobH = bounds.width;
        // Slider knob width: track width + side offset (limit to avoid becoming huge)
        float sideOffset = std::min(bounds.width, 12.0f); // Maximum 12px to the left and right
        float knobW = bounds.width + (sideOffset * 2.0f);

        // 2. SAFELY CALCULATE MOVEMENT RANGE
        float minY = bounds.y + (knobH * 0.5f);
        float maxY = bounds.y + bounds.height - (knobH * 0.5f);
        float trackLength = maxY - minY;

        // PROTECTION: If the range is too small for such a slider — abort calculation to avoid overflow
        if (trackLength <= 1.0f) return false;

        // 3. NORMALIZATION AND CALCULATION OF CURRENT POSITION
        int currentVal = std::clamp(*value, minVal, maxVal);

        // Normalize the value to the range 0.0f to 1.0f
        float normValue = static_cast<float>(currentVal - minVal) / static_cast<float>(maxVal - minVal);

        // Y-position of the knob center (top - max, bottom - min)
        float currentKnobCenterY = maxY - (normValue * trackLength);

        // Rectangle for the knob collision detection
        Rectangle knobBounds = {
            bounds.x - sideOffset,
            currentKnobCenterY - (knobH * 0.5f),
            knobW,
            knobH
        };

        // 4. COLLISIONS AND CLICKS
        bool isHoveredBounds = CheckCollisionPointRec(mousePos, bounds);
        bool isHoveredKnob = CheckCollisionPointRec(mousePos, knobBounds);

        if (isLeftPressed && (isHoveredBounds || isHoveredKnob)) {
            activeSliderId = currentSliderId;
        }

        if (isLeftReleased && activeSliderId == currentSliderId) {
            activeSliderId = nullptr;
        }

        bool valueChanged = false;

        // 5. HANDLING MOVEMENT
        if (isLeftDown && activeSliderId == currentSliderId) {
            float clampedMouseY = std::clamp(mousePos.y, minY, maxY);

            // Calculating the normalized value from Y-coordinate to 0.0f..1.0f
            float newNorm = (maxY - clampedMouseY) / trackLength;
            float rawValue = minVal + newNorm * static_cast<float>(maxVal - minVal);

            // Discretization by step
            int stepsCount = static_cast<int>(std::round((rawValue - minVal) / static_cast<float>(step)));
            int newVal = minVal + (stepsCount * step);
            newVal = std::clamp(newVal, minVal, maxVal);

            if (newVal != *value) {
                *value = newVal;
                valueChanged = true;
                currentVal = newVal;
            }

            // Recalculate for rendering in the current frame
            normValue = static_cast<float>(currentVal - minVal) / static_cast<float>(maxVal - minVal);
            currentKnobCenterY = maxY - (normValue * trackLength);
        }

        // 6. RENDERING
        int colorBG = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
        int colorBGpres = GuiGetStyle(BUTTON, BASE_COLOR_PRESSED);
        int colorBGactv = GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED);
        int colorLINE = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
        int colorTEXT = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);

        // A) TRACK BACKGROUND
        DrawRectangleRec(bounds, GetColor(colorBG));
        DrawRectangleLinesEx(bounds, 1.0f, GetColor(colorLINE));

        // B) SLIDER
        Rectangle finalKnobRect = {
            std::round(bounds.x - sideOffset),
            std::round(currentKnobCenterY - (knobH * 0.5f)),
            std::round(knobW),
            std::round(knobH)
        };

        Color knobColor = (activeSliderId == currentSliderId) ? GetColor(colorBGpres) :
            (isHoveredKnob ? GetColor(colorBGactv) : GetColor(colorBG));

        DrawRectangleRec(finalKnobRect, knobColor);
        DrawRectangleLinesEx(finalKnobRect, 1.0f, GetColor(colorLINE));

        // ) Text with current value
        const char* valText = TextFormat("%d", currentVal);

        // Font size proportional to slider height
        float fontSize = std::round(knobH * 0.65f);
        float spacing = 1.0f;

        Vector2 textSize = MeasureTextEx(font, valText, fontSize, spacing);

        Vector2 textPos = {
            std::round(finalKnobRect.x + (finalKnobRect.width - textSize.x) * 0.5f),
            std::round(finalKnobRect.y + (finalKnobRect.height - textSize.y) * 0.5f)
        };

        DrawTextEx(font, valText, textPos, fontSize, spacing, GetColor(colorTEXT));

        return valueChanged;
    }

    bool SliderInt_H(Rectangle bounds, int* value, int minVal, int maxVal, int step, Font font) {
        if (value == nullptr || minVal >= maxVal || step <= 0) return false;

        // Unique ID for capturing the element
        static void* activeSliderId = nullptr;
        void* currentSliderId = reinterpret_cast<void*>(value);

        Vector2 mousePos = GetMousePosition();
        bool isLeftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool isLeftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool isLeftReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        // 1. Knob Geometry (Slider Knob)
        // Height of the knob is strictly equal to the track's height
        float knobH = bounds.height;

        // Calculate the width of the knob based on the text of the current value (with minimum margin)
        int currentVal = std::clamp(*value, minVal, maxVal);
        const char* valText = TextFormat("%d", currentVal);

        float fontSize = std::round(knobH * 0.65f);
        float spacing = 1.0f;
        Vector2 textSize = MeasureTextEx(font, valText, fontSize, spacing);

        // Width of the knob: width of the text + horizontal paddings, but not less than the height of the button
        float padX = std::round(knobH * 0.4f);
        float knobW = std::max(knobH, textSize.x + (padX * 2.0f));

        // 2. Movement Range of Knob Center Along the X-Axis (Does not go beyond the bounds)
        float minX = bounds.x + (knobW * 0.5f);
        float maxX = bounds.x + bounds.width - (knobW * 0.5f);
        float trackLength = maxX - minX;

        // Protection: if the track's width is too small for such a slider
        if (trackLength <= 1.0f) return false;

        // 3. Calculation of New Value when Moving Mouse Horizontally
        float normValue = static_cast<float>(currentVal - minVal) / static_cast<float>(maxVal - minVal);
        float currentKnobCenterX = minX + (normValue * trackLength);

        // Knob bounds
        Rectangle knobBounds = {
            currentKnobCenterX - (knobW * 0.5f),
            bounds.y,
            knobW,
            knobH
        };

        // 4. Handling Clicks and Capturing
        bool isHoveredBounds = CheckCollisionPointRec(mousePos, bounds);
        bool isHoveredKnob = CheckCollisionPointRec(mousePos, knobBounds);

        if (isLeftPressed && (isHoveredBounds || isHoveredKnob)) {
            activeSliderId = currentSliderId;
        }

        if (isLeftReleased && activeSliderId == currentSliderId) {
            activeSliderId = nullptr;
        }

        bool valueChanged = false;

        // 5. Calculating New Value during Mouse Movement Horizontally
        if (isLeftDown && activeSliderId == currentSliderId) {
            float clampedMouseX = std::clamp(mousePos.x, minX, maxX);

            // Normalization 0.0f (left) .. 1.0f (right)
            float newNorm = (clampedMouseX - minX) / trackLength;
            float rawValue = minVal + newNorm * static_cast<float>(maxVal - minVal);

            // Discretization by step
            int stepsCount = static_cast<int>(std::round((rawValue - minVal) / static_cast<float>(step)));
            int newVal = minVal + (stepsCount * step);
            newVal = std::clamp(newVal, minVal, maxVal);

            if (newVal != *value) {
                *value = newVal;
                valueChanged = true;
                currentVal = newVal;

                // Recalculating text for the new value
                valText = TextFormat("%d", currentVal);
                textSize = MeasureTextEx(font, valText, fontSize, spacing);
            }

            // Updating frame coordinates
            normValue = static_cast<float>(currentVal - minVal) / static_cast<float>(maxVal - minVal);
            currentKnobCenterX = minX + (normValue * trackLength);
        }

        // 6. Drawing
        int colorBG = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
        int colorBGpres = GuiGetStyle(BUTTON, BASE_COLOR_PRESSED);
        int colorBGactv = GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED);
        int colorLINE = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
        int colorTEXT = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);
        // A) Track Background
        DrawRectangleRec(bounds, GetColor(colorBG));
        DrawRectangleLinesEx(bounds, 1.0f, GetColor(colorLINE));

        // B) Knob
        Rectangle finalKnobRect = {
            std::round(currentKnobCenterX - (knobW * 0.5f)),
            std::round(bounds.y),
            std::round(knobW),
            std::round(knobH)
        };

        Color knobColor = (activeSliderId == currentSliderId) ? GetColor(colorBGpres) :
            (isHoveredKnob ? GetColor(colorBGactv) : GetColor(colorBG));

        DrawRectangleRec(finalKnobRect, knobColor);
        DrawRectangleLinesEx(finalKnobRect, 1.0f, GetColor(colorLINE));

        // C) Text Centered on the Knob
        Vector2 textPos = {
            std::round(finalKnobRect.x + (finalKnobRect.width - textSize.x) * 0.5f),
            std::round(finalKnobRect.y + (finalKnobRect.height - textSize.y) * 0.5f)
        };

        DrawTextEx(font, valText, textPos, fontSize, spacing, GetColor(colorTEXT));

        return valueChanged;
    }

}

