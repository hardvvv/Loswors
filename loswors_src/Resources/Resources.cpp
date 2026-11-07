#include "Resources.hpp"

#include <vector>
#include <format>
#include <algorithm>

#include <nanojpeg.h>

#include "../CORE/Const.hpp"
#include "../CORE/Log.hpp"

#include "../Localization/LocalSys.hpp"

namespace {
    namespace ltxt {
        constexpr char FontsReloaded_FT[]   = "[ Resources ] >> Fonts reloaded for the language ({})";
        constexpr char TextureLoadDone[]    = "[ Resources ] >> All textures load from MEM to GPU VRAM";
    }
}

namespace {

    Texture2D Load_PNG(const unsigned char* filedata, int datalen) {
        Image myImage = LoadImageFromMemory(".png", filedata, datalen);
        Texture2D foret = LoadTextureFromImage(myImage);    // load in VRAM
        SetTextureFilter(foret, TEXTURE_FILTER_BILINEAR);   // Try ??
        UnloadImage(myImage);
        return foret;
    }

    Texture2D Load_JPG_Nano(const unsigned char* filedata, int datalen) {
        // 1. Initialize and decode the array in RAM
        njInit();
        nj_result_t res = njDecode(filedata, datalen);

        if (res != NJ_OK) {
            // TraceLog(LOG_ERROR, "NanoJPEG Decode Failed with code: %d", res);
            njDone();
            return { 0 };
        }

        // 2. Populate the Image structure for Raylib
        // NanoJPEG decodes JPG into RGB format (3 bytes per pixel)
        Image myImage = { 0 };
        myImage.data = njGetImage();
        myImage.width = njGetWidth();
        myImage.height = njGetHeight();
        myImage.mipmaps = 1;
        myImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8; // IMPORTANT: R8G8B8 (not RGBA)

        // 3. Load into VRAM
        Texture2D texture = LoadTextureFromImage(myImage);
        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);

        // 4. Free the internal nanojpeg buffer
        njDone();

        return texture;
    }

}

//                 _            _       
//      _ __  _ __(_)_   ____ _| |_ ___ 
//     | '_ \| '__| \ \ / / _` | __/ _ \
//     | |_) | |  | |\ V / (_| | ||  __/
//     | .__/|_|  |_| \_/ \__,_|\__\___|
// --- |_| -----------------------------------------------

// IMG
Image       Resources::program_icon;
// INTRO
Texture2D   Resources::BG;
Texture2D   Resources::Holder;
Texture2D   Resources::Key;
Texture2D   Resources::Z;
//DECODE
Texture2D   Resources::INPROG;
Texture2D   Resources::INPROG_HOVER;
Texture2D   Resources::INFILE;
Texture2D   Resources::BIN_Shuffle;
Texture2D   Resources::FileHash;
Texture2D   Resources::Argon2;
Texture2D   Resources::G_XOR_Shuffle;
Texture2D   Resources::I_XOR_Shuffle;
// THEME
Texture2D   Resources::tbg_1_coldfog;
Texture2D   Resources::tbg_2_greenrailway;
Texture2D   Resources::tbg_3_plantout;
Texture2D   Resources::tbg_4_road;

// PROGRAM
Texture2D   Resources::copy;
Texture2D   Resources::cros;
Texture2D   Resources::enter;
Texture2D   Resources::eye_close;
Texture2D   Resources::eye_open;
Texture2D   Resources::file_check;
Texture2D   Resources::ok_gal;
Texture2D   Resources::reload;
Texture2D   Resources::setting;
Texture2D   Resources::win_minim;
Texture2D   Resources::program_tx;
Texture2D   Resources::header_unit;
Texture2D   Resources::search;
Texture2D   Resources::edit;
Texture2D   Resources::drop_file;
// FONT
Font        Resources::FT_Main;
Font        Resources::FT_Main_BIGNAME;
Font        Resources::FT_MainMono;

//                  _     _ _      
//      _ __  _   _| |__ | (_) ___ 
//     | '_ \| | | | '_ \| | |/ __|
//     | |_) | |_| | |_) | | | (__ 
//     | .__/ \__,_|_.__/|_|_|\___|
// --- |_| -----------------------------------------------

// INTRO
#include "textures/intro_txs/BG_512.hpp"
#include "textures/intro_txs/Holder_256.hpp"
#include "textures/intro_txs/Key_256.hpp"
#include "textures/intro_txs/Z_128.hpp"
//DECODE
#include "textures/decode_txs/Argon2_512.hpp"
#include "textures/decode_txs/BIN_Shuffle_512.hpp"
#include "textures/decode_txs/FileHash_512.hpp"
#include "textures/decode_txs/G_XOR_Shuffle_512.hpp"
#include "textures/decode_txs/INFILE_dcd_256.hpp"
#include "textures/decode_txs/INPROG_dcd_256.hpp"
#include "textures/decode_txs/INPROG_HOVER_dcd_256.hpp"
#include "textures/decode_txs/I_XOR_Shuffle_512.hpp"
// THEME
#include "textures/themebg_txs/tbg_1_coldfog_43.hpp"
#include "textures/themebg_txs/tbg_2_greenrailway_43.hpp"
#include "textures/themebg_txs/tbg_3_plantout_43.hpp" 
#include "textures/themebg_txs/tbg_4_road_43.hpp"
// PROGRAM
#include "textures/prog_txs/Prog_Icon_64.hpp"
#include "textures/prog_txs/copy_64.hpp"
#include "textures/prog_txs/cros_128.hpp"
#include "textures/prog_txs/drop_file_512.hpp"
#include "textures/prog_txs/enter_128.hpp"
#include "textures/prog_txs/eye_close_64.hpp"
#include "textures/prog_txs/eye_open_64.hpp"
#include "textures/prog_txs/file_check_512.hpp"
#include "textures/prog_txs/ok_gal_128.hpp"
#include "textures/prog_txs/reload_64.hpp"
#include "textures/prog_txs/setting_128.hpp"
#include "textures/prog_txs/win_minim_128.hpp"
#include "textures/prog_txs/header_unit_64.hpp"
#include "textures/prog_txs/search_64.hpp"
#include "textures/prog_txs/edit_64.hpp"
// FONT
#include "fonts/main_font.hpp"
#include "fonts/main_font_mono.hpp"


void Resources::ReloadFont() {
    std::vector<int> codepoint = LocalSys::GetCodePointARR();

    if (FT_Main.texture.id > 0) {
        UnloadFont(FT_Main);
    }
    if (FT_Main_BIGNAME.texture.id > 0) {
        UnloadFont(FT_Main_BIGNAME);
    }
    if (FT_MainMono.texture.id > 0) {
        UnloadFont(FT_MainMono);
    }

    constexpr int fontSize = 64;
    // load by code point ------------------------------------------
    FT_Main = LoadFontFromMemory(".ttf", main_font, main_font_len, fontSize, codepoint.data(), static_cast<int>(codepoint.size()));
    SetTextureFilter(FT_Main.texture, TEXTURE_FILTER_BILINEAR);

    FT_MainMono = LoadFontFromMemory(".ttf", main_font_mono, main_font_mono_len, fontSize, codepoint.data(), static_cast<int>(codepoint.size()));
    SetTextureFilter(FT_MainMono.texture, TEXTURE_FILTER_BILINEAR);

    {
        std::string targetText = Const::PROGRAM_NAME;
        codepoint.clear();
        size_t i = 0;
        size_t len = targetText.length();
        while (i < len) {
            int bytesRead = 0;
            int cp = GetCodepointNext(&targetText[i], &bytesRead);
            if (cp == 0) break;

            codepoint.push_back(cp);
            i += bytesRead;
        }
        std::sort(codepoint.begin(), codepoint.end());
        codepoint.erase(std::unique(codepoint.begin(), codepoint.end()), codepoint.end());
        if (codepoint.empty()) {
            codepoint.push_back(32);
        }

        int fontSize = 150;
        FT_Main_BIGNAME = LoadFontFromMemory(
            ".ttf", main_font, main_font_len, fontSize, codepoint.data(), static_cast<int>(codepoint.size()));
        SetTextureFilter(FT_Main_BIGNAME.texture, TEXTURE_FILTER_BILINEAR);
    }

    GuiSetFont(FT_Main);

    Log::add(std::format(ltxt::FontsReloaded_FT, LocalSys::GetCurLangName()), L_INFO);

}
void Resources::LoadAll() {

    // IMG

    program_icon = LoadImageFromMemory(".png", Prog_Icon_64, Prog_Icon_64_len);

    // TEXTURE

    program_tx = Load_PNG(Prog_Icon_64, Prog_Icon_64_len);

    BG      = Load_PNG(BG_512, BG_512_len);
    Holder  = Load_PNG(Holder_256, Holder_256_len);
    Key     = Load_PNG(Key_256, Key_256_len);
    Z       = Load_PNG(Z_128, Z_128_len);

    INPROG          = Load_PNG(INPROG_dcd_256, INPROG_dcd_256_len);
    INPROG_HOVER    = Load_PNG(INPROG_HOVER_dcd_256, INPROG_HOVER_dcd_256_len);
    INFILE          = Load_PNG(INFILE_dcd_256, INFILE_dcd_256_len);
    BIN_Shuffle     = Load_PNG(BIN_Shuffle_512, BIN_Shuffle_512_len);
    FileHash        = Load_PNG(FileHash_512, FileHash_512_len);
    Argon2          = Load_PNG(Argon2_512, Argon2_512_len);
    G_XOR_Shuffle   = Load_PNG(G_XOR_Shuffle_512, G_XOR_Shuffle_512_len);
    I_XOR_Shuffle   = Load_PNG(I_XOR_Shuffle_512, I_XOR_Shuffle_512_len);

    tbg_1_coldfog       = Load_JPG_Nano(tbg_1_coldfog_43, tbg_1_coldfog_43_len);
    tbg_2_greenrailway  = Load_JPG_Nano(tbg_2_greenrailway_43, tbg_2_greenrailway_43_len);
    tbg_3_plantout      = Load_JPG_Nano(tbg_3_plantout_43, tbg_3_plantout_43_len);
    tbg_4_road          = Load_JPG_Nano(tbg_4_road_43, tbg_4_road_43_len);

    copy        = Load_PNG(copy_64, copy_64_len);
    cros        = Load_PNG(cros_128, cros_128_len);
    enter       = Load_PNG(enter_128, enter_128_len);
    eye_close   = Load_PNG(eye_close_64, eye_close_64_len);
    eye_open    = Load_PNG(eye_open_64, eye_open_64_len);
    file_check  = Load_PNG(file_check_512, file_check_512_len);
    ok_gal      = Load_PNG(ok_gal_128, ok_gal_128_len);
    reload      = Load_PNG(reload_64, reload_64_len);
    setting     = Load_PNG(setting_128, setting_128_len);
    win_minim   = Load_PNG(win_minim_128, win_minim_128_len);
    header_unit = Load_PNG(header_unit_64, header_unit_64_len);
    search      = Load_PNG(search_64, search_64_len);
    edit        = Load_PNG(edit_64, edit_64_len);
    drop_file   = Load_PNG(drop_file_512, drop_file_512_len);
    

    Log::add(ltxt::TextureLoadDone, L_INFO);
    
    // FONT
    ReloadFont();
}
void Resources::ClearData() {

    UnloadImage(program_icon);

    UnloadFont(FT_Main);
    UnloadFont(FT_MainMono);

}


Image Resources::GetIMG_program_icon() {
    return program_icon;
}
// INTRO
Texture2D Resources::GetIntroTX_BG() {
    return BG;
}
Texture2D Resources::GetIntroTX_Holder() {
    return Holder;
}
Texture2D Resources::GetIntroTX_Key() {
    return Key;
}
Texture2D Resources::GetIntroTX_Z() {
    return Z;
}
//DECODE
Texture2D Resources::GetDcdTX_INPROG() {
    return INPROG;
}
Texture2D Resources::GetDcdTX_INPROG_HOVER() {
    return INPROG_HOVER;
}
Texture2D Resources::GetDcdTX_INFILE() {
    return INFILE;
}
Texture2D Resources::GetDcdTX_BIN_Shuffle() {
    return BIN_Shuffle;
}
Texture2D Resources::GetDcdTX_FileHash() {
    return FileHash;
}
Texture2D Resources::GetDcdTX_Argon2() {
    return Argon2;
}
Texture2D Resources::GetDcdTX_G_XOR_Shuffle() {
    return G_XOR_Shuffle;
}
Texture2D Resources::GetDcdTX_I_XOR_Shuffle() {
    return I_XOR_Shuffle;
}
// THEME
Texture2D Resources::GetTbg_1_coldfog() {
    return tbg_1_coldfog;
}
Texture2D Resources::GetTbg_2_greenrailway() {
    return tbg_2_greenrailway;
}
Texture2D Resources::GetTbg_3_plantout() {
    return tbg_3_plantout;
}
Texture2D Resources::GetTbg_4_road() {
    return tbg_4_road;
}
// PROGRAM
Texture2D Resources::GetTX_copy() {
    return copy;
}
Texture2D Resources::GetTX_cros() {
    return cros;
}
Texture2D Resources::GetTX_enter() {
    return enter;
}
Texture2D Resources::GetTX_eye_close() {
    return eye_close;
}
Texture2D Resources::GetTX_eye_open() {
    return eye_open;
}
Texture2D Resources::GetTX_file_check() {
    return file_check;
}
Texture2D Resources::GetTX_ok_gal() {
    return ok_gal;
}
Texture2D Resources::GetTX_reload() {
    return reload;
}
Texture2D Resources::GetTX_setting() {
    return setting;
}
Texture2D Resources::GetTX_win_minim() {
    return win_minim;
}
Texture2D Resources::GetTX_program_icon() {
    return program_tx;
}
Texture2D Resources::GetTX_header_unit() {
    return header_unit;
}
Texture2D Resources::GetTX_search() {
    return search;
}
Texture2D Resources::GetTX_edit() {
    return edit;
}
Texture2D Resources::GetTX_drop_file() {
    return drop_file;
}
// FONT
Font Resources::GetFT_main_font() {
    return FT_Main;
}
Font Resources::GetFT_main_BIGNAME_font() {
    return FT_Main_BIGNAME;
}
Font Resources::GetFT_main_font_mono() {
    return FT_MainMono;
}
