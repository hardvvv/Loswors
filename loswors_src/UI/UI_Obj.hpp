#pragma once
#include <raylib.h>
#include <cstdint>


namespace UI_Obj{

    bool isPressInZ(Rectangle zone, bool& pp);
    void SetWH_minAR(float minAR, Rectangle& zone);


//      _____        _                
//     |_   _|____ _| |_ _  _ _ _ ___ 
//       | |/ -_) \ /  _| || | '_/ -_)
//       |_|\___/_\_\\__|\_,_|_| \___|
// ---------------------------------------------------------------------
    void DrawTexture_Color(Rectangle aa, Texture2D texture, Color overcolor);
    void DrawTexture_ColorRot(Rectangle aa, Texture2D texture, Color overcolor, float rot);

    void DrawTextCenter(Font font, const char* text, Rectangle zone, float fontSize, float spacing, Color color);
    void DrawText_FitCenter(Rectangle bounds, const char* text, Font font, Color color, float padding = 0.0f);

//      ___ _____ _  _ 
//     | _ )_   _| \| |
//     | _ \ | | | .` |
//     |___/ |_| |_|\_|
// ---------------------------------------------------------------------
    bool BTN_Image(Rectangle aa, Texture2D texture, Color overcolor, float padding_wh);
    bool BTN_Image_TX_SQRT_CENTER(Rectangle aa, Texture2D texture, Color overcolor, float padding = 0.0f);

    bool Hide_BTN_Texture(Rectangle aa, Texture2D texture, Color overcolor);
    //bool Hide_BTN_Texture180(Rectangle aa, Texture2D texture, Color overcolor);

    bool BTN_Expand_By_Text(Vector2 outPV, Vector2 inPV,
        const char* text, Font font, float btnH, float spacing, Rectangle* Real_z = nullptr);
    bool BTN_Text_FitCenter(Rectangle bounds, const char* text, Font font, float padding = 0.0f);

    void BTN_VizEye(Rectangle aa, Color overcolor, bool& isHide);
    void BTN_VizEye_Slider(Vector2 p_out, Vector2 p_in, float side, bool& isHide);
    bool BTN_X16(float pivotX, float pivotY, int p0123ul);

//      ___                _   
//     |_ _|_ _  _ __ _  _| |_ 
//      | || ' \| '_ \ || |  _|
//     |___|_||_| .__/\_,_|\__|
// ------------ |_| ----------------------------------------------------
    void Secure_Input(Rectangle aa, char* text, int maxSize, bool& is_active, bool is_hide);

    bool InputInt(Rectangle aa, int* value, char* textBuffer, int bufferSize, bool* editMode, int min, int max);

    // activeID = -1 if no one selected
    void InputPIN(Rectangle aa, int& activeID, uint8_t(&PIN)[4], bool is_hide);

    void InputMultiLine(
        Rectangle bounds, char* text, size_t maxTextSize,
        int& cursorLine, int& cursorCol, bool readOnly, bool& isActive,
        Vector2& scrollOffset, float fontSize, bool isScroll = true, Rectangle parentScissor = { 0, 0, 0, 0 }
    );

    void PATH_DIR(Rectangle aa, char* path, size_t path_len, bool& is_active, void (*clcPATHf)(), int fileID);

    bool SliderInt_W(Rectangle bounds, int* value, int minVal, int maxVal, int step, Font font);
    bool SliderInt_H(Rectangle bounds, int* value, int minVal, int maxVal, int step, Font font);
}