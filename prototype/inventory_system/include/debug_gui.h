// DebugGUI.h
#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <vector>

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
    };

    static GUIValues guiValues;
    static std::vector<bool> layerInfo;
    static float mapScale;
};
