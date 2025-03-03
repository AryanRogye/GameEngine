// DebugGUI.h
#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL_render.h>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h> 
#include <SDL_image.h> 
#include <string>
#include <map>
#include <SDL_clipboard.h>
#include <ctime>
#include "comfy_lib.h"
#include <string.h>

#include "entity/entity.h"
#include "entity/player.h"


class DebugGUI {
public:
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void ApplyGuiStyle();


    static void renderLogs();
    static void renderMapInfo();
    static void renderLayerInfo();
    static void renderTexturesInfo();
    static void renderPlayerInfo(SDL_Renderer* renderer);

    static void renderEntitySpriteOptions(Entity *entity, SDL_Renderer* renderer);
    static void renderEntitySpriteCreationMenu(Entity *entity, SDL_Renderer* renderer);
    static void Render(SDL_Renderer* renderer);

    struct GUIValues {

        std::string rendererName = "";
        int monitorWidth = 0;
        int monitorHeight = 0;
        bool vsync = true;

        std::string mapName = "";
        bool toggleGui = true;
        float *mapScale;
        bool showLayerInfo = false;
        bool colorForDifferentTexture = false;
        bool colorForDifferentLayer = false;
        bool drawGridOverTexture = false;
        std::vector<bool> layerInfo;
        std::vector<std::pair<std::string, ErrorCode>> debugLogs;

        // Current Layer Mouse is on
        int currentMouseLayer = -1;
        std::string currentTextureName = "";

        float selectedTileScreenX = 0;
        float selectedTileScreenY = 0;
        float selectedTileScreenWidth = 0;
        float selectedTileScreenHeight = 0;

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

    static void showSelectedSDLTexture(
        SDL_Texture* texture,
        float screenX, 
        float screenY, 
        float screenWidth, 
        float screenHeight, 
        int tileIndex,
        int columns,
        int tileWidth,
        int tileHeight
    );

    static void setMapScale(float *scale);

    static void addDebugLog(std::string log, std::vector<ErrorCode> = {});
    static void addDebugLog(std::string log, ErrorCode = ErrorCode::NONE);
};
