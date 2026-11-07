#include "Drager.hpp"

#include "../../CORE/Settings.hpp"
#include "../../CORE/Core.hpp"
#include "../../Resources/Resources.hpp"
#include "../UI_Obj.hpp"
#include "../../CORE/Encode/toUFT8.hpp"
#include "../../CORE/FS/FileSys.hpp"

#include "../../Localization/loc.hpp"

#include <cmath>
#include <string>
#include <format>
#include <vector>
#include <mutex>      // Mandatory for std::mutex and std::lock_guard
#include <algorithm>

#include <iostream> // delete

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDI

#define CloseWindow Win32_CloseWindow
#define ShowCursor Win32_ShowCursor

#include <windows.h>
#include <ole2.h>
#include <shellapi.h> // Mandatory for HDROP and DragQueryFileA

#undef CloseWindow
#undef ShowCursor


#define DND_DEBUG_LOG 0

#if DND_DEBUG_LOG
#define LOG_DND(msg) std::cout << "[DND Log] " << msg << std::endl
#define ERR_DND(msg) std::cerr << "[DND Error] " << msg << std::endl
#else
#define LOG_DND(msg)
#define ERR_DND(msg)
#endif

// ============================================================================
// WINDOWS OLE DRAG & DROP INTERNAL STATE
// ============================================================================
namespace {
    bool dragging = false;
    POINT dragPos{};

    // save static arr (no std::mutex è std::vector)
    char droppedFilesRaw[10][1024];
    int droppedFilesCount = 0;
    POINT dropPosBuffer{};
    bool pendingDrop = false;

    class DropTarget : public IDropTarget {
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
            if (!ppv) return E_POINTER;
            *ppv = nullptr;
            if (riid == IID_IUnknown || riid == IID_IDropTarget) {
                *ppv = this;
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
        ULONG STDMETHODCALLTYPE Release() override { return 1; }

        HRESULT STDMETHODCALLTYPE DragEnter(IDataObject*, DWORD, POINTL pt, DWORD* effect) override {
            LOG_DND("DragEnter triggered at (" << pt.x << ", " << pt.y << ")");
            dragging = true;
            dragPos = { pt.x, pt.y };
            if (effect) *effect = DROPEFFECT_COPY;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* effect) override {
            dragging = true;
            dragPos = { pt.x, pt.y };
            if (effect) *effect = DROPEFFECT_COPY;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DragLeave() override {
            LOG_DND("DragLeave triggered. Cursor left the window.");
            dragging = false;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* effect) override {
            LOG_DND("Drop event triggered at (" << pt.x << ", " << pt.y << ")");
            dragging = false;

            FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stg;
            ZeroMemory(&stg, sizeof(STGMEDIUM));

            if (FAILED(pDataObj->GetData(&fmt, &stg))) {
                ERR_DND("GetData failed!");
                if (effect) *effect = DROPEFFECT_NONE;
                return S_OK;
            }

            HDROP hDrop = static_cast<HDROP>(GlobalLock(stg.hGlobal));
            if (!hDrop) {
                ERR_DND("GlobalLock failed!");
                ReleaseStgMedium(&stg);
                if (effect) *effect = DROPEFFECT_NONE;
                return S_OK;
            }

            UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            LOG_DND("Files count: " << count);

            if (count > 0) {
                LOG_DND("Processing files with safe C-arrays...");

                droppedFilesCount = 0;
                UINT limit = (count > 10) ? 10 : count;

                for (UINT i = 0; i < limit; ++i) {
                    wchar_t wpath[1024];
                    if (DragQueryFileW(hDrop, i, wpath, 1024)) {
                        // safe write to static arr
                        int size = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, droppedFilesRaw[droppedFilesCount], 1024, nullptr, nullptr);
                        if (size > 0) {
                            LOG_DND("Added file: " << droppedFilesRaw[droppedFilesCount]);
                            droppedFilesCount++;
                        }
                    }
                }

                dropPosBuffer = { pt.x, pt.y };
                pendingDrop = (droppedFilesCount > 0);
                LOG_DND("Drop setup complete. pendingDrop = " << pendingDrop);
            }

            GlobalUnlock(stg.hGlobal);
            ReleaseStgMedium(&stg);

            if (effect) *effect = DROPEFFECT_COPY;
            return S_OK;
        }
    };

    DropTarget dropTarget;
    bool initialized = false;

    void InitDragDrop() {
        if (initialized) return;

        HWND hwnd = (HWND)GetWindowHandle();
        LOG_DND("InitDragDrop called. HWND: " << hwnd);

        if (!hwnd) {
            ERR_DND("HWND is NULL! Window not created yet?");
            return;
        }

        HRESULT hrOle = OleInitialize(nullptr);
        LOG_DND("OleInitialize result: 0x" << std::hex << hrOle << std::dec);

        if (FAILED(hrOle) && hrOle != 0x80010106L) {
            ERR_DND("OleInitialize FAILED");
            return;
        }

        HRESULT hrRevoke = RevokeDragDrop(hwnd);
        LOG_DND("RevokeDragDrop result: 0x" << std::hex << hrRevoke << std::dec);

        HRESULT hrRegister = RegisterDragDrop(hwnd, &dropTarget);
        LOG_DND("RegisterDragDrop result: 0x" << std::hex << hrRegister << std::dec);

        if (FAILED(hrRegister)) {
            ERR_DND("RegisterDragDrop FAILED!");
            OleUninitialize();
            return;
        }

        LOG_DND("Drag & Drop successfully registered!");
        initialized = true;
    }
}
#endif


constexpr float

    padFz_wh = 0.02f,

    lineW_wh = 0.0126f,     // ref inside zone

    TXpad12_wh = 0.14f,
    TXTpad12_wh = 0.22f,

    NODROP_TXT_y0 = 0.265f,
    NODROP_TXT_y1 = 0.16f,

    NODROP_TX_y0 = 0.38f,
    NODROP_TX_xy1_wh = 0.47f,

    NOPAD_cros_xy1_wh = 0.158f,

    t = 0;

#define RT_TEST 0 

#if RT_TEST
inline static void C(Rectangle rt) {
    DrawRectangleLinesEx(rt, 1.0f, RED);
}
#else
inline static void C(Rectangle) {}
#endif

// main draw namespace
namespace {

    void GetAllzone(Rectangle aa_baze, Rectangle& aa_onlyone, Rectangle& aa_left, Rectangle& aa_right) {
        float pad = std::round(std::min(aa_baze.width, aa_baze.height) * padFz_wh);
        float half_pad = std::round(pad * 0.5f);

        Rectangle lz = { aa_baze.x, aa_baze.y, std::round(aa_baze.width * 0.5f), aa_baze.height };
        Rectangle rz = { lz.x + lz.width, aa_baze.y, lz.width, aa_baze.height };

        lz.x += pad;        lz.y += pad;
        rz.x += half_pad;   rz.y += pad;
        lz.width -= (pad + half_pad);    lz.height -= pad * 2;
        rz.width -= (pad + half_pad);    rz.height -= pad * 2;

        aa_baze.x += pad;            aa_baze.y += pad;
        aa_baze.width -= pad * 2;    aa_baze.height -= pad * 2;

        aa_onlyone = aa_baze;
        aa_left = lz;
        aa_right = rz;
    }

    std::vector<std::string> files;
    Vector2 lastDropCursorPos = { 0.0f, 0.0f };
    inline bool PollDroppedFiles_CROSSPLATFORM() {
        files.clear();

#ifdef _WIN32
        if (pendingDrop) {
            LOG_DND("Transferring paths to Raylib format...");

            for (int i = 0; i < droppedFilesCount; i++) {
                files.push_back(std::string(droppedFilesRaw[i]));
            }

            HWND hwnd = (HWND)GetWindowHandle();
            POINT p = dropPosBuffer;
            ScreenToClient(hwnd, &p);
            lastDropCursorPos = { (float)p.x, (float)p.y };

            droppedFilesCount = 0;
            pendingDrop = false;
        }
#else
        if (IsFileDropped()) {
            FilePathList ffget = LoadDroppedFiles();
            for (size_t i = 0; i < ffget.count; i++) {
                files.push_back(ffget.paths[i]);
            }
            UnloadDroppedFiles(ffget);
            lastDropCursorPos = GetMousePosition();
        }
#endif

        return !files.empty();
    }

    void _processDrop_(Rectangle aa, Vector2 CursorPos) {

        LOG_DND("Entering _processDrop_...");

        if (!PollDroppedFiles_CROSSPLATFORM()) {
            LOG_DND("PollDroppedFiles_CROSSPLATFORM returned false. Aborting drop process.");
            return;
        }

        size_t fileCount = files.size();
        LOG_DND("Processing " << fileCount << " files on main thread.");

        Rectangle lz = {}, rz = {}, OnlyOne = {};
        GetAllzone(aa, OnlyOne, lz, rz);

        bool isDONE = false;

        auto writeTO = [](bool& isOK, void(*clear)(), char* dest, size_t destSize, const char* src) {
            if (!FileSys::Ping_DirExists(src)) {
                clear();
                snprintf(dest, destSize, "%s", src);
                isOK = true;
                LOG_DND("Successfully assigned file path to settings.");
            }
            else {
                ERR_DND("File path is a directory or does not exist!");
            }
        };

        if (Settings::is_2file) {
            if (fileCount >= 1) {
                if (CheckCollisionPointRec(CursorPos, lz)) {
                    writeTO(isDONE, Settings::ClearFPath0, Settings::path_f0, sizeof(Settings::path_f0), files[0].c_str());
                }
                else if (CheckCollisionPointRec(CursorPos, rz)) {
                    writeTO(isDONE, Settings::ClearFPath1, Settings::path_f1, sizeof(Settings::path_f1), files[0].c_str());
                }
            }
        }
        else if (CheckCollisionPointRec(CursorPos, OnlyOne)) {
            if (fileCount == 1) {
                writeTO(isDONE, Settings::ClearFPath0, Settings::path_f0, sizeof(Settings::path_f0), files[0].c_str());
                Settings::ClearFPath1();
            }
            else if (fileCount >= 2) {
                Settings::is_2file = true;

                if (files[0].ends_with(std::format("{}{}", "_1.", Const::LOSWORS_FILE_FORMAT)) ||
                    files[1].ends_with(std::format("{}{}", "_0.", Const::LOSWORS_FILE_FORMAT))) {
                    std::swap(files[0], files[1]);
                }

                writeTO(isDONE, Settings::ClearFPath0, Settings::path_f0, sizeof(Settings::path_f0), files[0].c_str());
                writeTO(isDONE, Settings::ClearFPath1, Settings::path_f1, sizeof(Settings::path_f1), files[1].c_str());
            }
        }

        if (isDONE) {
            LOG_DND("Drop processing completed successfully. Executing Core::Exit_CREATE_NEW()");
            Core::Exit_CREATE_NEW();
        }
        else {
            LOG_DND("Drop processing finished but isDONE is false.");
        }
    }
    
    inline void _drawNODROP(Rectangle aa) {
        float minS = std::min(aa.width, aa.height);
        Rectangle TXTz = { aa.x, aa.y + aa.height * NODROP_TXT_y0, aa.width, aa.height * NODROP_TXT_y1 };
        Rectangle TXz = {
            std::round(aa.x + (aa.width - minS * NODROP_TX_xy1_wh) * 0.5f),
            std::round(aa.y + aa.height * NODROP_TX_y0),
            minS * NODROP_TX_xy1_wh, minS * NODROP_TX_xy1_wh };
        float crosSZ = std::round(minS * NOPAD_cros_xy1_wh);
        Rectangle Xul = { aa.x,                     aa.y,                       crosSZ, crosSZ };
        Rectangle Xur = { aa.x + aa.width - crosSZ, aa.y,                       crosSZ, crosSZ };
        Rectangle Xdr = { aa.x + aa.width - crosSZ, aa.y + aa.height - crosSZ,  crosSZ, crosSZ };
        Rectangle Xdl = { aa.x,                     aa.y + aa.height - crosSZ,  crosSZ, crosSZ };

        
        auto drawX = [](Rectangle Xz, Texture2D TX) {
            DrawTexturePro(TX, { 0.0f, 0.0f, (float)TX.width, (float)TX.height }, Xz, { 0.0f,0.0f }, 0.0f, RED);
        };

        Texture2D crosTX = Resources::GetTX_cros();
        drawX(Xul, crosTX);         drawX(Xur, crosTX);
        drawX(Xdl, crosTX);         drawX(Xdr, crosTX);

        UI_Obj::DrawText_FitCenter(TXTz, loc::t.drager.nodrop, Resources::GetFT_main_font_mono(), RED, 0.4f);

        Texture2D fdropTX = Resources::GetTX_drop_file();
        DrawTexturePro(fdropTX, { 0.0f, 0.0f, (float)fdropTX.width, (float)fdropTX.height }, TXz, { 0.0f,0.0f }, 0.0f, GRAY);

    }
    
    inline void _drawDropZone(Rectangle zone, const char* text, bool isActive) {

        Color color = isActive ? BLUE : GRAY;

        float minS = std::min(zone.width, zone.height);
        float lineW = minS * lineW_wh;

        float TXsize = minS - (minS * TXpad12_wh * 2);
        Rectangle TXz = {
            zone.x + (zone.width - TXsize) * 0.5f,
            zone.y + (zone.height - TXsize) * 0.5f - TXsize * 0.16666f,
            TXsize, TXsize };
        Rectangle TXTz = { zone.x, TXz.y + TXz.height,
            zone.width, TXz.height * 0.33333f };

        // draw ---

        C(TXz); C(TXTz);

        DrawRectangleLinesEx(zone, lineW, color);

        Texture2D dropF = Resources::GetTX_drop_file();
        DrawTexturePro(dropF, { 0.0f, 0.0f, (float)dropF.width, (float)dropF.height }, TXz, { 0.0f,0.0f }, 0.0f, color);

        UI_Obj::DrawText_FitCenter(TXTz, text, Resources::GetFT_main_font_mono(), color, TXTpad12_wh);
    }
    void _draw_(Rectangle aa, Vector2 CursorPos, bool isCanDrop) {

        if (isCanDrop) {
            Rectangle lz = {};
            Rectangle rz = {};
            Rectangle OnlyOne = {};
            GetAllzone(aa, OnlyOne, lz, rz);

            if (Settings::is_2file) {
                _drawDropZone(lz, loc::t.drager.f0, CheckCollisionPointRec(CursorPos, lz));
                _drawDropZone(rz, loc::t.drager.f1, CheckCollisionPointRec(CursorPos, rz));
            }
            else {
                _drawDropZone(OnlyOne, loc::t.drager.onef, CheckCollisionPointRec(CursorPos, OnlyOne));
            }

        }
        else {
            _drawNODROP(aa);
        }
    }
}

bool Drager::Draw_END_Process(Rectangle aa, bool isCanDrop) {
#ifdef _WIN32
    InitDragDrop();

    if (pendingDrop) {
        HWND hwnd = (HWND)GetWindowHandle();
        POINT p = dropPosBuffer;
        ScreenToClient(hwnd, &p);
        Vector2 dropCursorPos = { (float)p.x, (float)p.y };

        if (CheckCollisionPointRec(dropCursorPos, aa)) {
            if (isCanDrop) {
                _processDrop_(aa, dropCursorPos);
            }
            else {
                // If dropping is not allowed, but the user has dropped something,
                // simply clear the buffer to avoid getting stuck in an infinite loop.
                PollDroppedFiles_CROSSPLATFORM();
            }
            return true;
        }
        else {
            PollDroppedFiles_CROSSPLATFORM();
        }
    }

    if (dragging) {
        HWND hwnd = (HWND)GetWindowHandle();
        POINT p = dragPos;

        if (!ScreenToClient(hwnd, &p)) return false;

        Vector2 CursorPos = { (float)p.x, (float)p.y };

        if (CheckCollisionPointRec(CursorPos, aa)) {
            _draw_(aa, CursorPos, isCanDrop);
            return true;
        }
    }

    return false;

#else
    // Linux / macOS
    if (IsFileDropped()) {
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, aa) && isCanDrop) {
            _processDrop_(aa, mousePos);
            return true;
        }
        else {
            PollDroppedFiles_CROSSPLATFORM();
        }
    }
    return false;
#endif
}