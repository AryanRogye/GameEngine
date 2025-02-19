/** 

This is my implimentation of loading a Tiled map into a SDL Game.

**/


#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <cstddef>
#include <iostream>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include "debug_gui.h"

using json = nlohmann::json;

struct TSDL_Layer
{
    std::string name;
    int width;
    int height;
    std::vector<int> data;
};
struct TSDL_Tileset
{
    int firstGid;
    std::string source;
};
/**
The TSDL_Tileset will have a source file which will be a .tsx xml file
which we will load and parse to get the tileset information.
    **/
struct TSDL_TilesetSource
{
    std::string name;
    int tileWidth;
    int tileHeight;
    int tileCount;
    int columns;
    std::string imagePath;
    int imageWidth;
    int imageHeight;
    SDL_Texture *texture = nullptr;
};
struct TSDL_TileMap
{
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    std::vector<TSDL_Layer> layers;    // Multiple layers
    std::vector<TSDL_Tileset> tilesets; // Tilesets used
    std::vector<TSDL_TilesetSource> tilesetSources; // Tilesets used
    int maxTileCount = 0;
};


class TSDL
{
private:
    static SDL_Color getTilesetColor(int tsxIndex)
    {
        // Predefined unique colors for TSX sources
        SDL_Color colors[] = {
            {255, 0, 0, 100},    // Red
            {0, 255, 0, 100},    // Green
            {0, 0, 255, 100},    // Blue
            {255, 255, 0, 100},  // Yellow
            {255, 0, 255, 100},  // Magenta
            {0, 255, 255, 100}   // Cyan
        };
        int numColors = sizeof(colors) / sizeof(colors[0]);
        return colors[tsxIndex % numColors]; // Cycle through colors
    }
public:
    /** 
    Load the map and store it inside the TSDL_TileMap Struct
    **/
    static bool loadMap(SDL_Renderer* renderer, TSDL_TileMap &tileMap, const std::string& jsonPath, const std::string& tsxPath);

    /**
    Load the Tileset Source from the .tsx file
    **/
    static bool loadTsx(TSDL_TileMap &tileMap, const std::string &path);

    /** 
    The tilesetsources already contain the image path so we can load the texture from there
    **/
    static bool loadTexture(SDL_Renderer *renderer,TSDL_TileMap &tileMap);

    /**
    Get the Texture from the tileset source
    **/
    static int getLayersSize(TSDL_TileMap &tileMap);
    
    static bool drawMap(
        SDL_Renderer* renderer,
        TTF_Font *font,
        std::vector<SDL_Texture*> fontNumbers,
        TSDL_TileMap &tileMap,
        int mouseX,
        int mouseY
    );
};

