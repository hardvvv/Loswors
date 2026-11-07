cmake_minimum_required(VERSION 3.15)

project(Loswors LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



if (WIN32)
    set(WIN_RESOURCES resource.rc)      # icon
endif()


add_executable(${PROJECT_NAME} WIN32 # com for terminal
    # ----------------- External -----------------
    "External/utf8proc-2.11.3/utf8proc.c"

    "External/libtinyfiledialogs-v2.9.3/tinyfiledialogs.c"
    "External/libtinyfiledialogs-v2.9.3/tinyfiledialogs.h"

    "External/raylib-6.0_win64_msvc16/include/raylib.h"
    "External/raygui-v5.0/raygui.h"

    "External/nanojpeg-v1.3/nanojpeg.c"
    "External/nanojpeg-v1.3/nanojpeg.h"


    # ----------------- Sources -----------------
    ${WIN_RESOURCES}
    main.cpp
    External_rguilib.cpp

    # CORE
    "CORE/Const.hpp"
    "CORE/Core.hpp"                             "CORE/Core.cpp"
    
    "CORE/Log.hpp"                              "CORE/Log.cpp"
    "CORE/SecureWipe.hpp"
    "CORE/Settings.hpp"                         "CORE/Settings.cpp"
    # CORE Encode
    "CORE/Encode/toUFT8.hpp"                    "CORE/Encode/toUFT8.cpp"
    "CORE/Encode/UTF8_to_NFC.hpp"               "CORE/Encode/UTF8_to_NFC.cpp"
    # CORE FS
    "CORE/FS/FileSys.hpp"                       "CORE/FS/FileSys.cpp"
    "CORE/FS/FileViewer.hpp"                    "CORE/FS/FileViewer.cpp"
    # CORE lwrs
    "CORE/lwrs/LwrsFileUnit.hpp"                "CORE/lwrs/LwrsFileUnit.cpp"
    "CORE/lwrs/UNVERSION.hpp"                   "CORE/lwrs/UNVERSION.cpp"
    # CORE lwrs Encryption
    "CORE/lwrs/Encryption/BinaryShuffle.hpp"    "CORE/lwrs/Encryption/BinaryShuffle.cpp"
    "CORE/lwrs/Encryption/FileHash.hpp"         "CORE/lwrs/Encryption/FileHash.cpp"
    "CORE/lwrs/Encryption/KDF.hpp"              "CORE/lwrs/Encryption/KDF.cpp"
    "CORE/lwrs/Encryption/XOR_Shuffle.hpp"      "CORE/lwrs/Encryption/XOR_Shuffle.cpp"
    # CORE MonoProg
    "CORE/MonoProg/MonoProg.hpp"                "CORE/MonoProg/MonoProg.cpp"
    # CORE Timer
    "CORE/Timer/timer.hpp"                      "CORE/Timer/timer.cpp"

    # Localization      
    "Localization/LocalSys.hpp"                 "Localization/LocalSys.cpp"

    # Resources
    "Resources/Resources.hpp"                   "Resources/Resources.cpp"

    #  UI
    "UI/NotifService.hpp"                       "UI/NotifService.cpp"
    "UI/UI.hpp"                                 "UI/UI.cpp"
    "UI/UI_Obj.hpp"                             "UI/UI_Obj.cpp"
    # UI Elements
    "UI/Elements/DownBar_PN.hpp"                "UI/Elements/DownBar_PN.cpp"
    "UI/Elements/Drager.hpp"                    "UI/Elements/Drager.cpp"
    "UI/Elements/FileParam.hpp"                 "UI/Elements/FileParam.cpp"
    "UI/Elements/PassGen.hpp"                   "UI/Elements/PassGen.cpp"
    "UI/Elements/PUBlock.hpp"                   "UI/Elements/PUBlock.cpp"
    "UI/Elements/Settings_PN.hpp"               "UI/Elements/Settings_PN.cpp"
    "UI/Elements/TimeBlock.hpp"                 "UI/Elements/TimeBlock.cpp"
    # UI States
    "UI/States/Decode_ST.hpp"                    "UI/States/Decode_ST.cpp"
    "UI/States/Intro_ST.hpp"                    "UI/States/Intro_ST.cpp"
    "UI/States/Main_ST.hpp"                     "UI/States/Main_ST.cpp"
    "UI/States/Start_ST.hpp"                    "UI/States/Start_ST.cpp"
    # UI Theme  
    "UI/Theme/Theme.hpp"                        "UI/Theme/Theme.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")


 if(MSVC)
    target_compile_options(Loswors PRIVATE /std:c++20)
else()
    target_compile_options(Loswors PRIVATE -std=c++20)
endif()


add_subdirectory(External/argon2)



add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/External/raylib-6.0_win64_msvc16/lib/raylib.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/External/raylib-6.0_win64_msvc16/include"
)



target_include_directories(${PROJECT_NAME} PRIVATE 
    External/libtinyfiledialogs-v2.9.3
    External/raygui-v5.0
    External/nanojpeg-v1.3
    External/raylib-6.0_win64_msvc16/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    argon2
    raylib
    winmm
    gdi32
    opengl32
    user32
    shell32
    ole32
    advapi32
)
