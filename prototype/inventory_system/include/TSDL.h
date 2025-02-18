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
    static bool
    loadMap(SDL_Renderer* renderer, TSDL_TileMap &tileMap, const std::string& jsonPath, const std::string& tsxPath)
    {
        std::ifstream file(jsonPath);
        // Open the file
        if (!file.is_open())
        {
            std::cerr << "❌ Error: Could not open the file: (" << jsonPath << ")" << std::endl;
            return false;
        }
        // Read the file
        json j;
        file >> j;
        // Close the File
        file.close();

        // Load Basic Information
        tileMap.width = j["width"];
        tileMap.height = j["height"];
        tileMap.tileWidth = j["tilewidth"];
        tileMap.tileHeight = j["tileheight"];

        // Load Tilesets

        for (auto &tileset : j["tilesets"])
        {
            // Create a Tileset Object
            TSDL_Tileset t;

            // Load Basic Information
            t.source = tileset["source"];
            t.firstGid = tileset["firstgid"];
            tileMap.tilesets.push_back(t);
        }

        // Load Layers
        for (auto &layer : j["layers"])
        {
            // Create a Layer Object
            TSDL_Layer l;
            
            // Load Basic Information
            l.name = layer["name"];
            l.width = layer["width"];
            l.height = layer["height"];
            l.data = layer["data"].get<std::vector<int>>();

            tileMap.layers.push_back(l);
        }

        // We Will load the tsx files right now
        if (!loadTsx(tileMap, tsxPath))
        {
            return false;
        }

        if (!loadTexture(renderer, tileMap))
        {
            return false;
        }

        std::cout << "✅Succesfully Loaded Json Into Map Struct "<< std::endl;
        /*std::cout << "Path: " << path << std::endl;*/
        std::cout << "SourceNum: " << tileMap.tilesets.size() << std::endl;
        for (int i = 0; i < tileMap.tilesets.size(); i++)
        {
            TSDL_Tileset tile = tileMap.tilesets[i];
            TSDL_TilesetSource tileSource = tileMap.tilesetSources[i];
            // End GID will be the firstGid + tileCount
            int endGid = tile.firstGid + tileSource.tileCount;
            tileMap.maxTileCount = endGid;

            std::cout << "Source:\t" << tile.source << " | " << tile.firstGid << " -> " << endGid << std::endl;
        }
        std::cout << "Max Size: " << tileMap.maxTileCount << std::endl;
        std::cout << "======================================" << std::endl;
        
        return true;
    }

    /**
    Load the Tileset Source from the .tsx file
    **/
    static bool loadTsx(TSDL_TileMap &tileMap, const std::string &path)
    {
        std::cout << "🔵 Loading Tileset Sources" << std::endl;
        // Tsx is a xml file there could be many inside tilesets so we need to parse it
        for (auto &t : tileMap.tilesets)
        {
            // Create a Tileset Source Object
            pugi::xml_document doc;

            // Load the file
            if (!doc.load_file(std::string(path + t.source).c_str()))
            {
                std::cerr << "❌ Error: Could not load the file: (" << path + t.source << ")" << std::endl;
                return false;
            }

            // find the tileset node
            pugi::xml_node tilesetNode = doc.child("tileset");
            if (!tilesetNode)
            {
                std::cerr << "❌ Error: Could not find the <tileset> node in (" << path + t.source << ")" << std::endl;
                return false;
            }

            TSDL_TilesetSource ts;
            // Store The Values
            ts.name = tilesetNode.attribute("name").as_string();
            ts.tileWidth = tilesetNode.attribute("tilewidth").as_int();
            ts.tileHeight = tilesetNode.attribute("tileheight").as_int();
            ts.tileCount = tilesetNode.attribute("tilecount").as_int();
            ts.columns = tilesetNode.attribute("columns").as_int();

            // Getting Image Path
            pugi::xml_node imageNode = tilesetNode.child("image");
            if (!imageNode)
            {
                std::cerr << "❌ Error: Could not find the <image> node in tileset " << ts.name << std::endl;
                return false;
            }

            ts.imagePath = imageNode.attribute("source").as_string();
            ts.imageWidth = imageNode.attribute("width").as_int();
            ts.imageHeight = imageNode.attribute("height").as_int();

            // Add to the vector
            tileMap.tilesetSources.push_back(ts);
            std::cout << "✅Succesfully Loaded Tileset: " << ts.name << std::endl;
        }
        std::cout << "======================================" << std::endl;
        return true;
    }

    /** 
    The tilesetsources already contain the image path so we can load the texture from there
    **/
    static bool loadTexture(SDL_Renderer *renderer,TSDL_TileMap &tileMap)
    {
        for (auto &ts : tileMap.tilesetSources)
        {
            // Get the Image Path
             std::string imagePath = ts.imagePath;

            SDL_Surface *surface = IMG_Load(imagePath.c_str());
            if (!surface)
            {
                std::cerr << "❌ Error: Could not load the image: (" << imagePath << ")" << std::endl;
                std::cerr << "❌ Error: " << IMG_GetError() << std::endl;
                return false;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture)
            {
                std::cerr << "❌ Error: Could not create the texture: (" << imagePath << ")" << std::endl;
                std::cerr << "❌ Error: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(surface);
                return false;
            }
            SDL_FreeSurface(surface);
            
            ts.texture = texture;
             std::cout << "✅Succesfully Loaded Texture: " << imagePath << std::endl;
        }
        std::cout << "======================================" << std::endl;
        return true;
    }

    /**
    Get the Texture from the tileset source
    **/
    static int getLayersSize(TSDL_TileMap &tileMap)
    {
        return tileMap.layers.size();
    }

    static bool drawMap(
        SDL_Renderer* renderer,
        TTF_Font *font,
        std::vector<SDL_Texture*> fontNumbers,
        TSDL_TileMap &tileMap,
        std::vector<bool> &layerInfo
    )
    {
        if (DebugGUI::guiValues.mapScale <= 0)
        {
            DebugGUI::guiValues.mapScale = 1;
        }
        /**
        So in this 0 is the bottom layer and the highest is the top layer we wanna
        draw the bottom layer first and start going up.
        **/

        for (int i = 0; i < tileMap.layers.size(); i++)
        {
            if (!layerInfo[i]) continue;
            for (int y = 0 ; y < tileMap.layers[i].height; y++)
            {
                for (int x = 0; x < tileMap.layers[i].width; x++)
                {
                    // This is the index of the tile we have to map onto the screen
                    int tileIndex = tileMap.layers[i].data[x + y * tileMap.layers[i].width];
                    if (tileIndex == 0) continue;

                    // Check if any layer above this has a higher number (>0) at the same position
                    // !OPTIMIZATION: This can be optimized by storing the highest tile index for each position
                    bool isCovered = false;
                    for (int j = i + 1; j < tileMap.layers.size(); j++)
                    {
                        int upperTile = tileMap.layers[j].data[x + y * tileMap.layers[j].width]; // Correct width usage
                        if (upperTile > 0)
                        {
                            isCovered = true;
                            break;
                        }
                    }
                    if (isCovered) continue;

                    // Make sure the tileIndex is in range
                    if (tileIndex >= fontNumbers.size()) continue;

                    // Get The Image Texture
                    // loop through the tileset sources and see if they match with the name of the tileset
                    SDL_Texture *texture = nullptr;
                    int sourceWidth;
                    int sourceHeight;
                    int maxColumns = 0;

                    int textureIndex = -1;
                    for (int t = 0; t < tileMap.tilesetSources.size(); t++)
                    {
                        TSDL_Tileset tileset = tileMap.tilesets[t];
                        TSDL_TilesetSource tilesetSource = tileMap.tilesetSources[t];
                        if (tileIndex >= tileset.firstGid && tileIndex < tileset.firstGid + tilesetSource.tileCount)
                        {
                            texture = tilesetSource.texture;
                            sourceWidth = tilesetSource.tileWidth;
                            sourceHeight = tilesetSource.tileHeight;
                            maxColumns = tilesetSource.columns;
                            tileIndex -= tileset.firstGid; // Adjust index
                            textureIndex = t;
                            break;
                        }
                    }
                    if (!texture) continue; // Skip if the texture wasn't created properly

                    // This is a Debug Info Map to show the tile numbers and dfiferent layers
                    if (DebugGUI::guiValues.showLayerInfo)
                    {
                        // Get the texture for this number
                        SDL_Texture *textTexture = fontNumbers[tileIndex];
                        if (!textTexture) continue; // Skip if the texture wasn't created properly

                        // Query texture size
                        int textW, textH;
                        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);

                        // Scale text size to match map scale
                        float scaledTextW = textW * DebugGUI::guiValues.mapScale;
                        float scaledTextH = textH * DebugGUI::guiValues.mapScale;

                        // Set position inside the tile (centered)
                        SDL_FRect textRect = {
                            (x + 0.5f) * tileMap.tileWidth * DebugGUI::guiValues.mapScale - (scaledTextW / 2.0f), 
                            (y + 0.5f) * tileMap.tileHeight * DebugGUI::guiValues.mapScale - (scaledTextH / 2.0f), 
                            scaledTextW, scaledTextH
                        };

                        // Create a rectangle for the tile boundary
                        SDL_FRect rect = {
                            x * tileMap.tileWidth * DebugGUI::guiValues.mapScale, 
                            y * tileMap.tileHeight * DebugGUI::guiValues.mapScale, 
                            tileMap.tileWidth * DebugGUI::guiValues.mapScale, 
                            tileMap.tileHeight * DebugGUI::guiValues.mapScale
                        };

                        if (DebugGUI::guiValues.colorForDifferentLayer && DebugGUI::guiValues.colorForDifferentTexture)
                        {
                            DebugGUI::guiValues.colorForDifferentTexture = false;
                            DebugGUI::guiValues.colorForDifferentLayer = false;
                        }

                        // Get the tileset color dynamically (only if colorForDifferentTsx is enabled)
                        if (DebugGUI::guiValues.colorForDifferentTexture)
                        {
                            SDL_Color color = getTilesetColor(textureIndex);
                            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                            SDL_RenderFillRectF(renderer, &rect); // Fill background color
                        }

                        if (DebugGUI::guiValues.colorForDifferentLayer)
                        {
                            // Get the layer color dynamically
                            SDL_Color color = getTilesetColor(i);
                            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                            SDL_RenderFillRectF(renderer, &rect); // Fill background color
                        }

                        // Draw the white border
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawRectF(renderer, &rect);

                        // Render the tile number in the center
                        SDL_RenderCopyF(renderer, textTexture, NULL, &textRect);
                    }
                    // Showing The Map
                    else 
                    {
                        // Calculate the source rectangle (inside the tileset image)
                        SDL_Rect srcRect = { 
                            (tileIndex % maxColumns) * sourceWidth,   // X position in the tileset
                            (tileIndex / maxColumns) * sourceHeight,  // Y position in the tileset
                            sourceWidth, 
                            sourceHeight
                        };

                        // Calculate the destination rectangle (on screen)
                        SDL_FRect destRect = {
                            x * tileMap.tileWidth * float(DebugGUI::guiValues.mapScale), 
                            y * tileMap.tileHeight * float(DebugGUI::guiValues.mapScale), 
                            tileMap.tileWidth * float(DebugGUI::guiValues.mapScale), 
                            tileMap.tileHeight * float(DebugGUI::guiValues.mapScale)
                        };

                        // Render the tile with floating-point precision
                        SDL_RenderCopyF(renderer, texture, &srcRect, &destRect);

                        // draw grid if enabled
                        if (DebugGUI::guiValues.drawGridOverTexture)
                        {
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderDrawRectF(renderer, &destRect);
                        }
                    }
                }
            }
        }
        return true;
    }
};

