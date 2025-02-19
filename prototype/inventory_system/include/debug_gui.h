// DebugGUI.h
#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include "comfy_lib.h"
#include <SDL_clipboard.h>
#include <ctime>

class DebugGUI {
public:
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void ApplyGuiStyle();
    static void Render(SDL_Renderer* renderer);

    struct GUIValues {
        std::string mapName = "";
        bool toggleGui = true;
        float *mapScale;
        bool showLayerInfo = false;
        bool colorForDifferentTexture = false;
        bool colorForDifferentLayer = false;
        bool drawGridOverTexture = false;
        std::vector<bool> layerInfo;
        std::vector<std::pair<std::string, bool>> debugLogs;

        // Current Layer Mouse is on
        int currentMouseLayer = -1;
        std::string currentTextureName = "";
        // Current x and y of the tile of which the mouse is on
        int currentMouseTileX = -1;
        int currentMouseTileY = -1;
        SDL_Texture *currentTileTexture;

        // =====================================================================================================================
        // Player Info
        // =====================================================================================================================
        Player* player;
    };

    static void SetPlayer(Player* player);
    static void SetMapName(const std::string& mapName);
    static GUIValues guiValues;
    static std::vector<bool> layerInfo;
    static void addDebugLog(const std::string& log, bool copyClipboard = false ,const std::string& label="");
    static void showSelectedSDLTexture(SDL_Texture* texture, int x, int y, int w, int h);
    static void setMapScale(float *scale);
};
