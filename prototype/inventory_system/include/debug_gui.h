// DebugGUI.h
#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <vector>
#include <string>
#include <map>
#include "comfy_lib.h"
#include <SDL_clipboard.h>

class DebugGUI {
public:
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void Render(SDL_Renderer* renderer);

    struct GUIValues {
        bool toggleGui = true;
        float mapScale = 3.0;
        bool showLayerInfo = false;
        bool colorForDifferentTexture = false;
        bool colorForDifferentLayer = false;
        bool drawGridOverTexture = false;
        std::vector<bool> layerInfo;
        std::vector<std::pair<std::string, bool>> debugLogs;
    };

    static GUIValues guiValues;
    static std::vector<bool> layerInfo;
    static void addDebugLog(const std::string& log, bool copyClipboard = false ,const std::string& label="");
};
