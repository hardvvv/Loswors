/*
This singleton was supposed to clearly bind parameters to a specific individual element on the screen
(while resetting the settings to zero when calling the "test__;" command),
but I ran into the problem that too many parameters needed to be changed with each call,
they were often repeated, so I thought that I would first make the first default theme and
then add constants to the palette if I need to use different colors for different designs
(I am writing this to explain the inconsistency of my code now, there are parts here that can be corrected
from the point of view of the structure and purity of the code (as in the whole project),
but I honestly do not want to spend a lot of time on this, at least for now)
*/

#include "Theme.hpp"

#include <raygui.h>
#include <stdlib.h>

#include <cmath>
#include <ctime>

#include "../../CORE/Settings.hpp"
#include "../../Resources/Resources.hpp"


#define IS_RANDOM_CHECK 0


/*
	0_genesis.hpp - have example of structure that other palettes files rewrite,
	every other palettes include "0_genesis.hpp" inside self
*/
#include "palettes/0_genesis.hpp"
#include "palettes/1_coldfog.hpp"
#include "palettes/2_greenrailway.hpp"
#include "palettes/3_plantout.hpp"
#include "palettes/4_road.hpp"

namespace {
    constexpr size_t t_count = 5;
    constexpr palette ccc[t_count] = {
        p0_genesis,
        p1_coldfog,
        p2_greenrailway,
        p3_plantout,
        p4_road
    };
    int t_id = -1;

    palette CP = ccc[0]; // curent palette

    void ALLRANDOM() {

        auto randcolor = []() -> int {
            Color c = {
                (unsigned char)(rand() % 256),
                (unsigned char)(rand() % 256),
                (unsigned char)(rand() % 256),
                255
            };
            return ColorToInt(c);
            };
        auto ApplyChaosToBaseProperties = [&](int control) {
            // 1. Все базовые состояния цвета (Normal, Focused, Pressed, Disabled)
            GuiSetStyle(control, BORDER_COLOR_NORMAL, randcolor());
            GuiSetStyle(control, BASE_COLOR_NORMAL, randcolor());
            GuiSetStyle(control, TEXT_COLOR_NORMAL, randcolor());

            GuiSetStyle(control, BORDER_COLOR_FOCUSED, randcolor());
            GuiSetStyle(control, BASE_COLOR_FOCUSED, randcolor());
            GuiSetStyle(control, TEXT_COLOR_FOCUSED, randcolor());

            GuiSetStyle(control, BORDER_COLOR_PRESSED, randcolor());
            GuiSetStyle(control, BASE_COLOR_PRESSED, randcolor());
            GuiSetStyle(control, TEXT_COLOR_PRESSED, randcolor());

            GuiSetStyle(control, BORDER_COLOR_DISABLED, randcolor());
            GuiSetStyle(control, BASE_COLOR_DISABLED, randcolor());
            GuiSetStyle(control, TEXT_COLOR_DISABLED, randcolor());

            // 2. Базовая геометрия и отступы
            GuiSetStyle(control, BORDER_WIDTH, rand() % 3);   // 0-2px max!
            GuiSetStyle(control, TEXT_PADDING, rand() % 4);   // 0-3px max!
            GuiSetStyle(control, TEXT_ALIGNMENT, rand() % 3);     // LEFT(0), CENTER(1), RIGHT(2)
            };

        const int controls[] = {
            DEFAULT,
            LABEL,
            BUTTON,
            TOGGLE,
            SLIDER,
            PROGRESSBAR,
            CHECKBOX,
            COMBOBOX,
            DROPDOWNBOX,
            TEXTBOX,
            VALUEBOX,
            TABBAR,
            LISTVIEW,
            COLORPICKER,
            SCROLLBAR,
            STATUSBAR
        };

        // 1. Применяем базовые свойства ко ВСЕМ контролам из enum
        for (int control : controls) {
            ApplyChaosToBaseProperties(control);
        }
        GuiSetFont((rand() % 2 == 0) ? Resources::GetFT_main_font() : Resources::GetFT_main_font_mono());

        // 2. Расширенные глобальные свойства (GuiDefaultProperty для DEFAULT)
        GuiSetStyle(DEFAULT, TEXT_SIZE, 10 + rand() % 24); // 10-33px
        GuiSetStyle(DEFAULT, TEXT_SPACING, rand() % 10);
        GuiSetStyle(DEFAULT, LINE_COLOR, randcolor());
        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, randcolor());
        GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, 10 + rand() % 20);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, rand() % 3);       // TOP(0), MIDDLE(1), BOTTOM(2)
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, rand() % 3);       // NONE(0), CHAR(1), WORD(2)

        // 3. Специфичные свойства для каждого контрола (GuiControl Extended Properties)

        // Toggle / ToggleGroup
        GuiSetStyle(TOGGLE, GROUP_PADDING, rand() % 12);
        GuiSetStyle(TOGGLE, GROUP_WIDTH_FULL, rand() % 2);

        // Slider / SliderBar
        GuiSetStyle(SLIDER, SLIDER_WIDTH, 5 + rand() % 30);
        GuiSetStyle(SLIDER, SLIDER_PADDING, rand() % 10);

        // ProgressBar
        GuiSetStyle(PROGRESSBAR, PROGRESS_PADDING, rand() % 10);
        GuiSetStyle(PROGRESSBAR, PROGRESS_SIDE, rand() % 2);

        // CheckBox
        GuiSetStyle(CHECKBOX, CHECK_PADDING, 1 + rand() % 10);

        // ComboBox
        GuiSetStyle(COMBOBOX, COMBO_BUTTON_WIDTH, 10 + rand() % 30);
        GuiSetStyle(COMBOBOX, COMBO_BUTTON_SPACING, rand() % 10);

        // DropdownBox
        GuiSetStyle(DROPDOWNBOX, ARROW_PADDING, rand() % 10);
        GuiSetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_SPACING, rand() % 10);
        GuiSetStyle(DROPDOWNBOX, DROPDOWN_ARROW_HIDDEN, rand() % 2);
        GuiSetStyle(DROPDOWNBOX, DROPDOWN_ROLL_UP, rand() % 2);

        // TextBox / ValueBox / Spinner
        GuiSetStyle(TEXTBOX, TEXT_READONLY, rand() % 2);

        // ValueBox / Spinner
        GuiSetStyle(VALUEBOX, SPINNER_BUTTON_WIDTH, 10 + rand() % 30);
        GuiSetStyle(VALUEBOX, SPINNER_BUTTON_SPACING, rand() % 10);

        // TabBar
        GuiSetStyle(TABBAR, TAB_ITEMS_WIDTH, 40 + rand() % 100);
        GuiSetStyle(TABBAR, TAB_CLOSE_BUTTON, rand() % 2);
        GuiSetStyle(TABBAR, TAB_LINE_SIDE, rand() % 2);

        //// ListView
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 15 + rand() % 30);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_SPACING, rand() % 10);
        GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 8 + rand() % 20);
        GuiSetStyle(LISTVIEW, SCROLLBAR_SIDE, rand() % 2);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_BORDER_NORMAL, rand() % 2);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_BORDER_WIDTH, rand() % 4);

        // ColorPicker
        GuiSetStyle(COLORPICKER, COLOR_SELECTOR_SIZE, 8 + rand() % 20);
        GuiSetStyle(COLORPICKER, HUEBAR_WIDTH, 10 + rand() % 25);
        GuiSetStyle(COLORPICKER, HUEBAR_PADDING, rand() % 10);
        GuiSetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT, 2 + rand() % 10);
        GuiSetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW, rand() % 2);

        // ScrollBar
        GuiSetStyle(SCROLLBAR, ARROWS_SIZE, 8 + rand() % 16);
        GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, rand() % 2);
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, rand() % 8);
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, 10 + rand() % 40);
        GuiSetStyle(SCROLLBAR, SCROLL_PADDING, rand() % 10);
        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, 1 + rand() % 20);
    }

}
#if IS_RANDOM_CHECK
    #define test__ ALLRANDOM();
#else
    #define test__ ((void)0)
#endif


int Theme::GetThemeCount() {
    return static_cast<int>(t_count);
}

void Theme::SetTheme(int id) {
    auto ReloadTheme = [&]() {
        CP = ccc[t_id];

        auto ApplyDefParam = [&](int control) {

            GuiSetStyle(control, BORDER_COLOR_NORMAL, ColorToInt(CP.def_line_norm));
            GuiSetStyle(control, BASE_COLOR_NORMAL, ColorToInt(CP.def_bg_norm));
            GuiSetStyle(control, TEXT_COLOR_NORMAL, ColorToInt(CP.def_text_norm));

            GuiSetStyle(control, BORDER_COLOR_FOCUSED, ColorToInt(CP.def_line_actv));
            GuiSetStyle(control, BASE_COLOR_FOCUSED, ColorToInt(CP.def_bg_actv));
            GuiSetStyle(control, TEXT_COLOR_FOCUSED, ColorToInt(CP.def_text_actv));

            GuiSetStyle(control, BORDER_COLOR_PRESSED, ColorToInt(CP.def_line_pres));
            GuiSetStyle(control, BASE_COLOR_PRESSED, ColorToInt(CP.def_bg_pres));
            GuiSetStyle(control, TEXT_COLOR_PRESSED, ColorToInt(CP.def_text_pres));

            GuiSetStyle(control, BORDER_COLOR_DISABLED, ColorToInt(CP.def_line_norm));
            GuiSetStyle(control, BASE_COLOR_DISABLED, ColorToInt(CP.def_bg_norm));
            GuiSetStyle(control, TEXT_COLOR_DISABLED, ColorToInt(CP.def_text_norm));


            GuiSetStyle(control, BORDER_WIDTH, 1);
            GuiSetStyle(control, TEXT_PADDING, 1);
            GuiSetStyle(control, TEXT_ALIGNMENT, 1);
        };

        const int controls[] = {
            DEFAULT,
            LABEL,
            BUTTON,
            TOGGLE,
            SLIDER,
            PROGRESSBAR,
            CHECKBOX,
            COMBOBOX,
            DROPDOWNBOX,
            TEXTBOX,
            VALUEBOX,
            TABBAR,
            LISTVIEW,
            COLORPICKER,
            SCROLLBAR,
            STATUSBAR
        };

        // 1. Применяем базовые свойства ко ВСЕМ контролам из enum
        for (int control : controls) {
            ApplyDefParam(control);
        }
        GuiSetFont(Resources::GetFT_main_font());

        GuiSetStyle(DEFAULT, TEXT_SIZE, 5);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
        GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, 10);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, 1);       // TOP(0), MIDDLE(1), BOTTOM(2)
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, 0);       // NONE(0), CHAR(1), WORD(2)


        // Toggle / ToggleGroup
        GuiSetStyle(TOGGLE, GROUP_PADDING, 1);

        // Slider / SliderBar
        GuiSetStyle(SLIDER, SLIDER_WIDTH, 16);
        GuiSetStyle(SLIDER, SLIDER_PADDING, 0);

        // CheckBox
        GuiSetStyle(CHECKBOX, CHECK_PADDING, 1);

        // TextBox / ValueBox / Spinner
        GuiSetStyle(TEXTBOX, TEXT_READONLY, 0);

        // ScrollBar
        GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, 0);
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, 1);
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, 16);
        GuiSetStyle(SCROLLBAR, SCROLL_PADDING, 1);

    };

    if (t_id == -1 && id == -1) {
        // first init rand (enter program)
        std::srand(static_cast<unsigned int>(std::time(nullptr))); // without it, i get the same val. at start
        t_id = (rand() % t_count);
        Settings::ui_theme_id = t_id;
        ReloadTheme();
        return;
    }
    if (t_id != -1 && id == -1) {
        // incorrect input
        return;
    }
    if (id >= 0 && id < t_count) {
        // normal work
        t_id = id;
        ReloadTheme();
        return;
    }
}

void Theme::DrawBG(Rectangle aa, int id, bool isDim) {
    float bgAR = 1.33333f; // 4/3
    float zAR = aa.width / ((aa.height != 0.0f) ? aa.height : 1.0f);
    bool isRefW = zAR > bgAR;

    bool isIDOK = (id >= 0 && id < t_count);

    Rectangle drawz = aa;
    if (!isIDOK) {
        if (isRefW) {
            drawz = {
                aa.x,
                std::ceil(aa.y + (aa.height - aa.width * 0.75f) * 0.5f),
                aa.width,
                std::ceil(aa.width * 0.75f)
            };
        }
        else {
            drawz = {
                std::ceil(aa.x + (aa.width - aa.height * bgAR) * 0.5f),
                aa.y,
                std::ceil(aa.height * bgAR),
                aa.height
            };
        }
    }
    

    auto drawTX = [&](Texture2D bgTX, Color overcolor) {
        DrawTexturePro(bgTX, { 0.0f, 0.0f, (float)bgTX.width, (float)bgTX.height }, drawz, { 0.0f, 0.0f }, 0.0f, overcolor);
    };

    int drawID = (isIDOK) ? id : t_id;

    switch (drawID)
    {
    case 0:
        DrawRectangle(drawz.x, drawz.y, drawz.width, drawz.height, { 41, 44, 51, 255 });
        break;
    case 1:
        drawTX(Resources::GetTbg_1_coldfog(), isDim ? Color{ 140, 140, 140, 255 } : Color{ 230, 230, 230, 255 });
        break;
    case 2:
        drawTX(Resources::GetTbg_2_greenrailway(), isDim ? Color{ 255, 255, 255, 255 } : Color{ 255, 255, 255, 255 });
        break;
    case 3:
        drawTX(Resources::GetTbg_3_plantout(), isDim ? Color{ 210, 210, 210, 255 } : Color{ 255, 255, 255, 255 });
        break;
    case 4:
        drawTX(Resources::GetTbg_4_road(), isDim ? Color{ 180, 180, 180, 255 } : Color{ 215, 215, 215, 255 });
        break;
    default:
        break;
    }
}

//       ___     _            
//      / __|___| |___ _ _ ___
//     | (__/ _ \ / _ \ '_(_-<
//      \___\___/_\___/_| /__/
// ---------------------------------------------------------

const Color& Theme::C::log_norm = CP.log_norm;
const Color& Theme::C::log_ok = CP.log_ok;
const Color& Theme::C::log_warning = CP.log_warning;
const Color& Theme::C::log_error = CP.log_error;
const Color& Theme::C::logs_bg_def = CP.logs_bg_def;
const Color& Theme::C::logs_bg_dark = CP.logs_bg_dark;
const Color& Theme::C::log_fullscreen_bg = CP.def_bg_norm;
const Color& Theme::C::log_fullscreen_line = CP.def_line_norm;

const Color& Theme::C::setting_inactive_txt = CP.setting_inactive_text_norm;
const Color& Theme::C::setting_lang_txt = CP.setting_checkbox_text_norm;
const Color& Theme::C::setting_fullhd_txt = CP.setting_fullhd_text;
const Color& Theme::C::setting_fullhd_bg = CP.setting_fullhd_bg;
const Color& Theme::C::setting_div_line = CP.setting_div_line;

const Color& Theme::C::mst_fparcopy_bg_old = CP.mst_fp_fparcopy_bg_old;
const Color& Theme::C::mst_fparcopy_txt_old = CP.mst_fp_fparcopy_txt_old;
const Color& Theme::C::mst_fparcopy_bg_new = CP.mst_fp_fparcopy_bg_new;
const Color& Theme::C::mst_fparcopy_txt_new = CP.mst_fp_fparcopy_txt_new;
const Color& Theme::C::mst_fp_fpar_bg_warning = CP.mst_fp_fpar_bg_warning;
const Color& Theme::C::mst_fp_fpar_txt_warning = CP.mst_fp_fpar_txt_warning;
const Color& Theme::C::mst_fparcopy_txt = CP.mst_fp_fparcopy_txt;

const Color& Theme::C::timeblock_border = CP.timeblock_border;

const Color& Theme::C::dirview_file = CP.dirview_file;

const Color& Theme::C::passgen_0_rt = CP.passgen_0_rt;
const Color& Theme::C::passgen_1_rt = CP.passgen_1_rt;

const Color& Theme::C::global_bg = CP.bg;



//      ___ ___         ___      _     ___ _        _     
//     / __/ __|  ___  / __| ___| |_  / __| |_ _  _| |___ 
//     \__ \__ \ |___| \__ \/ -_)  _| \__ \  _| || | / -_)
//     |___/___/       |___/\___|\__| |___/\__|\_, |_\___|
// ------------------------------------------- |__/ --------

namespace {
    void SetBtnStyle(
        float textH, int lineW,
        Color normTXT, Color normLINE, Color normBG,
        Color actvTXT, Color actvLINE, Color actvBG,
        Color presTXT, Color presLINE, Color presBG
    ) {
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(normTXT));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(normBG));

        GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(actvTXT));
        GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(actvLINE));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(actvBG));

        GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(presTXT));
        GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(presLINE));
        GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(presBG));

        //GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED, ColorToInt(normTXT));
        //GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED, ColorToInt(normLINE));
        //GuiSetStyle(BUTTON, BASE_COLOR_DISABLED, ColorToInt(normBG));

        
        GuiSetStyle(BUTTON, BORDER_WIDTH, lineW);
        GuiSetStyle(BUTTON, TEXT_PADDING, 4);
        GuiSetStyle(DEFAULT, TEXT_SIZE, textH);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
    }

    void SetScrollStyle(
        int padLR, int padSlider,
        Color LRline, Color bg, Color sliderBG,
        Color sliderNORM, Color sliderACTV, Color sliderPRES
    ) {
        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(sliderBG));
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(sliderNORM));
        GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(sliderACTV));
        GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(sliderPRES));
        
        GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, ColorToInt(LRline));

        GuiSetStyle(SCROLLBAR, SCROLL_PADDING, padLR);
        GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, 0);
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, padSlider);

        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(bg));
        GuiSetStyle(DEFAULT, BORDER_WIDTH, 0);

        GuiSetStyle(LISTVIEW, BORDER_WIDTH, 0);
        GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 16);
        GuiSetStyle(LISTVIEW, SCROLLBAR_SIDE, SCROLLBAR_RIGHT_SIDE);
    }

    void SetCheckBoxStyle(
        int lineW, int ckPAD, float textH, float textPAD, float textcharpad,
        Color normTXT, Color normLINE,
        Color actvTXT, Color actvLINE,
        Color presTXT, Color presLINE
    ) {
        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(normTXT));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(actvTXT));
        GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(presTXT));

        GuiSetStyle(CHECKBOX, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(CHECKBOX, BORDER_COLOR_FOCUSED, ColorToInt(actvLINE));
        GuiSetStyle(CHECKBOX, BORDER_COLOR_PRESSED, ColorToInt(presLINE));


        GuiSetStyle(DEFAULT, TEXT_SPACING, textcharpad);
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, BORDER_WIDTH, lineW);
        GuiSetStyle(DEFAULT, TEXT_SIZE, textH);
        GuiSetStyle(CHECKBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
        GuiSetStyle(CHECKBOX, CHECK_PADDING, ckPAD);
        GuiSetStyle(CHECKBOX, TEXT_PADDING, textPAD);
    }

    void SetTextBoxStyle(
        float textH, int lineW,
        Color normTXT, Color normLINE, Color normBG,
        Color actvTXT, Color actvLINE, Color actvBG,
        Color presTXT, Color presLINE, Color presBG
    ) {
        GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(normTXT));
        GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, ColorToInt(normBG));

        GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(actvTXT));
        GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(actvLINE));
        GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, ColorToInt(actvBG));

        GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(presTXT));
        GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, ColorToInt(presLINE));
        GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, ColorToInt(presBG));

        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, textH);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
        GuiSetStyle(TEXTBOX, BORDER_WIDTH, lineW);
        GuiSetStyle(TEXTBOX, TEXT_PADDING, lineW + lineW);
        GuiSetStyle(TEXTBOX, TEXT_READONLY, 0);

        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    }

    void SetListViewStyle(
        float textH, int lineW, float unitH, int spacing,
        Color normTXT, Color normLINE, Color normBG,
        Color actvTXT, Color actvLINE, Color actvBG,
        Color presTXT, Color presLINE, Color presBG
    ) {
        GuiSetStyle(LISTVIEW, TEXT_COLOR_NORMAL, ColorToInt(normTXT));
        GuiSetStyle(LISTVIEW, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(LISTVIEW, BASE_COLOR_NORMAL, ColorToInt(normBG));

        GuiSetStyle(LISTVIEW, TEXT_COLOR_FOCUSED, ColorToInt(actvTXT));
        GuiSetStyle(LISTVIEW, BORDER_COLOR_FOCUSED, ColorToInt(actvLINE));
        GuiSetStyle(LISTVIEW, BASE_COLOR_FOCUSED, ColorToInt(actvBG));

        GuiSetStyle(LISTVIEW, TEXT_COLOR_PRESSED, ColorToInt(presTXT));
        GuiSetStyle(LISTVIEW, BORDER_COLOR_PRESSED, ColorToInt(presLINE));
        GuiSetStyle(LISTVIEW, BASE_COLOR_PRESSED, ColorToInt(presBG));

        /*
        How could GUI be created so stupid that several parameters overlap and prevent
        the ability to customize the element's appearance!??
        For example, how am I supposed to even know which element has which specific parameters?
        Why not create a proper list for each element so that the IDE can autocomplete?
        How badly must the constant system have been designed that I have to manually use BINARY SEARCH,
        disabling element randomization, to find which parameter affects what...
        */

        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_MIDDLE);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
        GuiSetStyle(DEFAULT, BORDER_WIDTH, lineW);
        GuiSetStyle(DEFAULT, TEXT_SIZE, textH);
        GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(normLINE));
        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(normBG));
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, unitH);
        GuiSetStyle(LISTVIEW, BORDER_WIDTH, lineW);
        GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, static_cast<int>(textH * 0.5f));
        GuiSetStyle(LISTVIEW, LIST_ITEMS_SPACING, spacing);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_BORDER_NORMAL, 1); // 0 off 1 on
        GuiSetStyle(LISTVIEW, LIST_ITEMS_BORDER_WIDTH, lineW);

        GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, 0);


        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(presBG));
        GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(presBG));

        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(normBG));

        GuiSetStyle(SCROLLBAR, SCROLL_PADDING, 0);
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, 0);

    }

    void ToggleSlider(
        float textH,
        Color normTXT, Color normLINE, Color normBG,
        Color actvTXT, Color actvLINE, Color actvBG,
        Color presTXT, Color presLINE, Color presBG
    ) {
        GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, ColorToInt(normBG));

        GuiSetStyle(SLIDER, BORDER_COLOR_FOCUSED, ColorToInt(actvLINE));
        GuiSetStyle(SLIDER, BASE_COLOR_FOCUSED, ColorToInt(actvBG));

        GuiSetStyle(SLIDER, BORDER_COLOR_PRESSED, ColorToInt(presLINE));
        GuiSetStyle(SLIDER, BASE_COLOR_PRESSED, ColorToInt(presBG));


        GuiSetStyle(SLIDER, BORDER_WIDTH, 1);
        //GuiSetStyle(BUTTON, TEXT_PADDING, 4);
        GuiSetStyle(DEFAULT, TEXT_SIZE, textH);
        //GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);



        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(normTXT));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(actvTXT));
        GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(presTXT));

        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(normBG));
        GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(actvBG));
        GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(presBG));

        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(normLINE));
        GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(actvLINE));
        GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(presLINE));
        GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, ColorToInt(normLINE));


        //GuiSetStyle(SLIDER, SLIDER_WIDTH, 5 + rand() % 30);
        GuiSetStyle(SLIDER, SLIDER_PADDING, 0);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
        
        //GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
    }

}



void Theme::SS_db_log(Color txtColor, float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    SetBtnStyle(txtH, 1,
        txtColor, CP.def_line_norm, CP.def_bg_norm,
        txtColor, CP.def_line_actv, CP.def_bg_actv,
        txtColor, CP.def_line_pres, CP.def_bg_pres
    );   
}
void Theme::SS_db_version(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    SetBtnStyle(txtH, 1,
        CP.db_version_text, CP.def_line_norm, CP.def_bg_norm,
        CP.db_version_text, CP.def_line_actv, CP.def_bg_actv,
        CP.db_version_text, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_db_exit(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    SetBtnStyle(txtH, 1,
        CP.db_exit_text, CP.def_line_norm, CP.def_bg_norm,
        CP.db_exit_text, CP.def_line_actv, CP.def_bg_actv,
        CP.db_exit_text, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_db_log_scroll() {
    test__;
    SetScrollStyle(0, 0, PINK, CP.log_scroll_bg, CP.def_bg_norm, CP.def_line_norm, CP.def_bg_actv, CP.def_bg_pres);
}
void Theme::SS_db_version_scroll() {
    test__;
    SetScrollStyle(0, 0, PINK, CP.log_scroll_bg, CP.def_bg_norm, CP.def_line_norm, CP.def_bg_actv, CP.def_bg_pres);
}
void Theme::SS_db_version_scroll_btns() {
    test__;
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(Color{ 180,180,180,255 }));

    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
}

void Theme::SS_setting_main_btn() {
    test__;
    SetBtnStyle(0, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_setting_checkbox(float txtH, float textPAD) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetCheckBoxStyle(
        2, 2, txtH, textPAD, 1,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_norm,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_actv,
        CP.setting_checkbox_text_pres, CP.setting_checkbox_line_pres
    );
}
void Theme::SS_setting_input_timeout(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetTextBoxStyle(
        txtH, 1,
        CP.def_text_norm, CP.setting_inactive_text_norm, BLANK,
        CP.def_bg_pres, CP.setting_inactive_text_norm, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_setting_theme_choose() {
    test__;
    SetCheckBoxStyle(
        2, 2, 1, 1, 1,
        CP.setting_checkbox_text_norm, CP.setting_theme_checkbox_line_norm,
        CP.setting_checkbox_text_norm, CP.setting_theme_checkbox_line_actv,
        CP.setting_checkbox_text_pres, CP.setting_theme_checkbox_line_pres
    );
    SetBtnStyle(1, 1,
        PINK, CP.def_line_norm, CP.def_bg_norm,
        PINK, CP.def_line_norm, CP.def_bg_norm,
        PINK, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_setting_lang_choose_btn(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    SetBtnStyle(txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_setting_lang_choose_list(float txtH, float unitH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetListViewStyle(txtH, 0, unitH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_setting_setdef_btn(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    SetBtnStyle(txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_setting_reloadsetwh_btn() {
    test__;
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}

void Theme::SS_sst_checkbox(float txtH, float txtpad) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetCheckBoxStyle(
        1, 1, txtH, txtpad, 0,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_norm,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_actv,
        CP.setting_checkbox_text_pres, CP.setting_checkbox_line_pres
    );
}
void Theme::SS_sst_12file(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    ToggleSlider(
        txtH,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_sst_newbtn() {
    test__;
    //GuiSetFont(Resources::GetFT_main_font());
    //GuiSetStyle(, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_sst_makenew(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetTextBoxStyle(
        txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_sst_passinput(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetTextBoxStyle(
        txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_sst_fdraw(float txtH) {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetTextBoxStyle(
        txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_sst_XX() {
    test__;
    SetBtnStyle(
        GuiGetStyle(DEFAULT, TEXT_SIZE), 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}

void Theme::SS_mst_file_info() {
    test__;
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);
    SetScrollStyle(
        0, 0,
        PINK, {0,0,0,90}, CP.def_bg_norm,
        CP.def_line_norm, CP.def_bg_actv, CP.def_bg_pres
    );
}
void Theme::SS_mst_search_checkbox(float txtH, float txtpad) {
    test__;
    SetCheckBoxStyle(
        1, 1, txtH, txtpad, 0,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_norm,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_actv,
        CP.setting_checkbox_text_pres, CP.setting_checkbox_line_pres
    );
}
void Theme::SS_mst_save_btn(bool isActive) {
    test__;
    if (isActive) {
        SetBtnStyle(1, 1,
            CP.def_text_norm, CP.def_line_norm, CP.mst_save_btn_bg_active,
            CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
            CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
        );
    }
    else {
        Color text  = CP.def_text_norm; text.a  = 100;
        Color line  = CP.def_line_norm; line.a  = 100;
        Color bg    = CP.def_bg_norm;   bg.a    = 100;
        SetBtnStyle(1, 1,
            text, line, bg,
            text, line, bg,
            text, line, bg
        );
    }
}
void Theme::SS_mst_fparam_btn() {
    test__;
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_puarr_scroll() {
    test__;
    SetScrollStyle(0, 0, PINK, BLANK, CP.def_bg_norm, CP.def_line_norm, CP.def_bg_actv, CP.def_bg_pres);
}
void Theme::SS_mst_pu_block() {
    test__;

    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetTextBoxStyle(1, 1, 
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_newbtn() {
    test__;
    GuiSetFont(Resources::GetFT_main_font());
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_search(float txtH) {
    test__;
    SetBtnStyle(
        1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetTextBoxStyle(
        txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_generator(float txtH) {
    test__;

    SetScrollStyle(0, 0, PINK, BLANK, CP.def_bg_norm, CP.def_line_norm, CP.def_line_norm, CP.def_line_norm);

    SetBtnStyle(
        1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetTextBoxStyle(
        txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_fp_pinverif() {
    test__;
    SetTextBoxStyle(
        1, 1, 
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    ToggleSlider(
        1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_fp_fparcopy_btn_act() {
    test__;
    SetBtnStyle(
        1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.mst_fp_fparcopy_btn_bg_act,
        CP.def_text_norm, CP.def_line_norm, CP.mst_fp_fparcopy_btn_bg_act,
        CP.def_text_norm, CP.def_line_norm, CP.mst_fp_fparcopy_btn_bg_act
    );
}
void Theme::SS_mst_fp_fparcopy_btn_pas() {
    test__;
    SetBtnStyle(
        1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_fp_fparcopy_filedata(float txtH, float txtpad) {
    test__;
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetTextBoxStyle(txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetCheckBoxStyle(1, 1, txtH, txtpad, 1,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_norm,
        CP.setting_checkbox_text_norm, CP.setting_checkbox_line_actv,
        CP.setting_checkbox_text_pres, CP.setting_checkbox_line_pres
    );
}
void Theme::SS_mst_fp_fparcopy_secretdata() {
    test__;
    SetBtnStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetTextBoxStyle(1, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
void Theme::SS_mst_fp_editpu(float txtH) {
    test__;

    SetScrollStyle(0, 0, PINK, BLANK, CP.def_bg_norm, CP.def_line_norm, CP.def_bg_actv, CP.def_bg_pres);

    SetBtnStyle(txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_actv, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
    SetTextBoxStyle(txtH, 1,
        CP.def_text_norm, CP.def_line_norm, CP.def_bg_norm,
        CP.def_text_norm, CP.def_line_actv, CP.def_bg_actv,
        CP.def_text_pres, CP.def_line_pres, CP.def_bg_pres
    );
}
