#include "TSDL.h"

    /** 
    Load the map and store it inside the TSDL_TileMap Struct
    **/
    bool
    TSDL::loadMap(SDL_Renderer* renderer, TSDL_TileMap *tileMap, const std::string& jsonPath, const std::string& tsxPath)
    {
        std::ifstream file(jsonPath);
        // Open the file
        if (!file.is_open())
        {
            DebugGUI::addDebugLog("Error: Could not open the file:", false, "Json");
            DebugGUI::addDebugLog(jsonPath, true);
            return false;
        }

        // Read the file
        json j;
        file >> j;
        // Close the File
        file.close();

        // Load Basic Information
        tileMap->width = j.at("width");
        tileMap->height = j.at("height");
        tileMap->tileWidth = j.at("tilewidth");
        tileMap->tileHeight = j.at("tileheight");

        // Load Tilesets

        for (auto &tileset : j.at("tilesets"))
        {
            // Create a Tileset Object
            TSDL_Tileset t;

            // Load Basic Information
            t.source = tileset.at("source");
            t.firstGid = tileset.at("firstgid");

            tileMap->tilesets.push_back(t);
        }

        // Load Layers
        for (auto &layer : j.at("layers"))
        {
            // Create a Layer Object
            TSDL_Layer l;

            // Load Basic Information
            l.name = layer.at("name");

            // Only assign width and height if it's a tile layer
            if (layer.contains("width") && layer.contains("height"))
            {
                l.width = layer.at("width");
                l.height = layer.at("height");
            }
            else
            {
                // If it's an object layer, set width/height to 0 or handle differently
                l.width = 0;
                l.height = 0;
            }

            // Only parse "data" if it's present (Tile layers have data, object layers do not)
            if (layer.contains("data"))
            {
                l.data = layer.at("data").get<std::vector<int>>();
            }

            tileMap->layers.push_back(l);
        }

        // We Will load the tsx files right now
        // this will be whaterver ......assets/......
        // we wanna remove everything after the last /
        std::string path = tsxPath;
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            path = path.substr(0, lastSlash + 1);
        }
        else
        {
            path = "";
        }
        if (!loadTsx(tileMap, path))
        {
            DebugGUI::addDebugLog("Error: Could not load the tsx file: (" + path + ")", false, "TSDL");
            DebugGUI::addDebugLog(path, true, "TSDL");
            return false;
        }

        if (!loadTexture(renderer, tileMap))
        {
            DebugGUI::addDebugLog("Error: Could not load the texture from the tsx file: (" + path + ")", false, "TSDL");
            DebugGUI::addDebugLog(path, true, "TSDL");
            return false;
        }

        DebugGUI::addDebugLog("Succesfully Loaded Json Into Map Struct", false, "TSDL");
        /*std::cout << "Path: " << path << std::endl;*/
        DebugGUI::addDebugLog("Sources: " + std::to_string(tileMap->tilesets.size()), false, "TSDL");
        for (int i = 0; i < tileMap->tilesets.size(); i++)
        {
            TSDL_Tileset tile = tileMap->tilesets[i];
            TSDL_TilesetSource tileSource = tileMap->tilesetSources[i];
            // End GID will be the firstGid + tileCount
            int endGid = tile.firstGid + tileSource.tileCount;
            tileMap->maxTileCount = endGid;

            DebugGUI::addDebugLog("Source:\t" + tile.source + " | " + std::to_string(tile.firstGid) + " -> " + std::to_string(endGid), false, "TSDL");
        }
        DebugGUI::addDebugLog("Max Size: " + std::to_string(tileMap->maxTileCount), false, "TSDL");
        std::cout << "======================================" << std::endl;
        
        return true;
    }

    /**
    Load the Tileset Source from the .tsx file
    **/
    bool TSDL::loadTsx(TSDL_TileMap *tileMap, const std::string &path)
    {
        // Tsx is a xml file there could be many inside tilesets so we need to parse it
        for (auto &t : tileMap->tilesets)
        {
            std::string tempPath = path;
            // Create a Tileset Source Object
            pugi::xml_document doc;

            // Load the file
            if (!doc.load_file(std::string(tempPath + t.source).c_str()))
            {
                std::cerr << "❌ Error: Could not load the file: (" << tempPath + t.source << ")" << std::endl;
                return false;
            }

            // find the tileset node
            pugi::xml_node tilesetNode = doc.child("tileset");
            if (!tilesetNode)
            {
                std::cerr << "❌ Error: Could not find the <tileset> node in (" << tempPath + t.source << ")" << std::endl;
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
            tileMap->tilesetSources.push_back(ts);
            DebugGUI::addDebugLog("Succesfully Loaded Tsx: " + ts.name, false, "TSDL");
        }
        return true;
    }

    /** 
    The tilesetsources already contain the image path so we can load the texture from there
    **/
    bool TSDL::loadTexture(SDL_Renderer *renderer,TSDL_TileMap *tileMap)
    {
        for (auto &ts : tileMap->tilesetSources)
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
    int TSDL::getLayersSize(TSDL_TileMap &tileMap)
    {
        return tileMap.layers.size();
    }

    bool TSDL::drawMap(
        SDL_Renderer* renderer,
        TTF_Font *font,
        std::vector<SDL_Texture*> fontNumbers,
        TSDL_TileMap *tileMap,
        int mouseX,
        int mouseY,
        float mapScale
    )
    {
        if (mapScale <= 0)
        {
            mapScale = 1;
        }
        /**
        So in this 0 is the bottom layer and the highest is the top layer we wanna
        draw the bottom layer first and start going up.
        **/

        // ==========================================================================================================================
        // Iterating Layers
        // ==========================================================================================================================
        for (int i = 0; i < tileMap->layers.size(); i++)
        {
            // ==========================================================================================================================
            // Debug Checking : Showing Layer Info
            // ==========================================================================================================================
            if (!DebugGUI::guiValues.layerInfo[i]) continue;
            // ==========================================================================================================================
            // Iterating Tiles Horizontally
            // ==========================================================================================================================
            for (int y = 0 ; y < tileMap->layers[i].height; y++)
            {
                // ==========================================================================================================================
                // Iterating Tiles Vertically
                // ==========================================================================================================================
                for (int x = 0; x < tileMap->layers[i].width; x++)
                {
                    // ==========================================================================================================================
                    // This is the index of the tile we have to map onto the screen
                    // ==========================================================================================================================
                    int tileIndex = tileMap->layers[i].data[x + y * tileMap->layers[i].width];
                    if (tileIndex == 0) continue;

                    // ==========================================================================================================================
                    // Check if any layer above this has a higher number (>0) at the same position
                    // !OPTIMIZATION: This can be optimized by storing the highest tile index for each position
                    // ==========================================================================================================================
                    bool isCovered = false;
                    for (int j = i + 1; j < tileMap->layers.size(); j++)
                    {
                        int upperTile = tileMap->layers[j].data[x + y * tileMap->layers[j].width]; // Correct width usage
                        if (upperTile > 0)
                        {
                            isCovered = true;
                            break;
                        }
                    }
                    if (isCovered) continue;

                    // ==========================================================================================================================
                    // Make sure the tileIndex is in range
                    // ==========================================================================================================================
                    if (tileIndex >= fontNumbers.size()) continue;

                    // ==========================================================================================================================
                    // Get The Image Texture
                    // loop through the tileset sources and see if they match with the name of the tileset
                    // ==========================================================================================================================
                    SDL_Texture *texture = nullptr;
                    int sourceWidth;
                    int sourceHeight;
                    int maxColumns = 0;

                    int textureIndex = -1;
                    int offset = 0;
                    for (int t = 0; t < tileMap->tilesetSources.size(); t++)
                    {
                        TSDL_Tileset tileset = tileMap->tilesets[t];
                        TSDL_TilesetSource tilesetSource = tileMap->tilesetSources[t];
                        if (tileIndex >= tileset.firstGid && tileIndex < tileset.firstGid + tilesetSource.tileCount)
                        {
                            texture = tilesetSource.texture;
                            sourceWidth = tilesetSource.tileWidth;
                            sourceHeight = tilesetSource.tileHeight;
                            maxColumns = tilesetSource.columns;
                            tileIndex -= tileset.firstGid; // Adjust index
                            offset = tileset.firstGid;
                            textureIndex = t;
                            break;
                        }
                    }
                    if (!texture) continue; // Skip if the texture wasn't created properly

                    // ==========================================================================================================================
                    // Calculate the mouse position in tile coordinates
                    // ==========================================================================================================================
                    int tileX = mouseX / (tileMap->tileWidth * mapScale);
                    int tileY = mouseY / (tileMap->tileHeight * mapScale);

                    // ==========================================================================================================================
                    // Debug Mouse related Texture/Layer Info
                    // ==========================================================================================================================
                    if (tileX == x && tileY == y)
                    {
                        DebugGUI::guiValues.currentMouseLayer = i;
                        DebugGUI::guiValues.currentTextureName = tileMap->tilesetSources[textureIndex].name;

                        DebugGUI::guiValues.currentMouseTileX = x;
                        DebugGUI::guiValues.currentMouseTileY = y;
                    }

                    // ==========================================================================================================================
                    // Debug Info !!!! In Production this will be disabled
                    // ==========================================================================================================================
                    if (DebugGUI::guiValues.showLayerInfo)
                    {
                        tileIndex += offset; // Adjust index back for the debug info

                        // ==========================================================================================================================
                        // Get the font tetxture for the tile number
                        // ==========================================================================================================================
                        SDL_Texture *textTexture = fontNumbers[tileIndex];
                        if (!textTexture) continue; // Skip if the texture wasn't created properly

                        // ==========================================================================================================================
                        // Get the text size
                        // ==========================================================================================================================
                        int textW, textH;
                        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);

                        // ==========================================================================================================================
                        // Scale text size to match map scale
                        // ==========================================================================================================================
                        float scaledTextW = textW * mapScale;
                        float scaledTextH = textH * mapScale;

                        // ==========================================================================================================================
                        // Set position inside the tile (centered)
                        // ==========================================================================================================================
                        SDL_FRect textRect = {
                            (x + 0.5f) * tileMap->tileWidth *  mapScale - (scaledTextW / 2.0f), 
                            (y + 0.5f) * tileMap->tileHeight * mapScale - (scaledTextH / 2.0f), 
                            scaledTextW, scaledTextH
                        };

                        // ==========================================================================================================================
                        // Create a rectangle for the tile boundary
                        // ==========================================================================================================================
                        SDL_FRect rect = {
                            x * tileMap->tileWidth *  mapScale, 
                            y * tileMap->tileHeight * mapScale, 
                            tileMap->tileWidth *      mapScale, 
                            tileMap->tileHeight *     mapScale
                        };

                        // ==========================================================================================================================
                        // Debug <- This will Cancel the colorForDifferentLayer and colorForDifferentTexture
                        // if both are enabled
                        // ==========================================================================================================================
                        if (DebugGUI::guiValues.colorForDifferentLayer && DebugGUI::guiValues.colorForDifferentTexture)
                        {
                            DebugGUI::guiValues.colorForDifferentTexture = false;
                            DebugGUI::guiValues.colorForDifferentLayer = false;
                        }

                        // ==========================================================================================================================
                        // Different Files that are used for the map will have different colors
                        // Get the tileset color dynamically (only if colorForDifferentTsx is enabled)
                        // ==========================================================================================================================
                        if (DebugGUI::guiValues.colorForDifferentTexture)
                        {
                            SDL_Color color = getTilesetColor(textureIndex);
                            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                            SDL_RenderFillRectF(renderer, &rect); // Fill background color
                        }

                        // ==========================================================================================================================
                        // Different Layers will have different colors
                        // Get the layer color dynamically
                        // ==========================================================================================================================
                        if (DebugGUI::guiValues.colorForDifferentLayer)
                        {
                            // Get the layer color dynamically
                            SDL_Color color = getTilesetColor(i);
                            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                            SDL_RenderFillRectF(renderer, &rect); // Fill background color
                        }

                        // ==========================================================================================================================
                        // Draw the white border <- Shows Up Always in Debug Mode
                        // ==========================================================================================================================
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawRectF(renderer, &rect);

                        // ==========================================================================================================================
                        // Render the tile number in the center
                        // ==========================================================================================================================
                        SDL_RenderCopyF(renderer, textTexture, NULL, &textRect);
                    }
                    // ==========================================================================================================================
                    // ==========================================================================================================================
                    // ==========================================================================================================================
                    // Showing The Map !!!! <Production>
                    // ==========================================================================================================================
                    // ==========================================================================================================================
                    // ==========================================================================================================================
                    else 
                    {
                        // ==========================================================================================================================
                        // Calculate the source rectangle (inside the tileset image)
                        // ==========================================================================================================================
                        SDL_Rect srcRect = { 
                            (tileIndex % maxColumns) * sourceWidth,   // X position in the tileset
                            (tileIndex / maxColumns) * sourceHeight,  // Y position in the tileset
                            sourceWidth, 
                            sourceHeight
                        };

                        // ==========================================================================================================================
                        // Calculate the destination rectangle (on screen)
                        // ==========================================================================================================================
                        SDL_FRect destRect = {
                            x * tileMap->tileWidth *  float(mapScale), 
                            y * tileMap->tileHeight * float(mapScale), 
                            tileMap->tileWidth *      float(mapScale), 
                            tileMap->tileHeight *     float(mapScale)
                        };

                        // ==========================================================================================================================
                        // Render the tile with floating-point precision
                        // ==========================================================================================================================
                        SDL_RenderCopyF(renderer, texture, &srcRect, &destRect);

                        // ==========================================================================================================================
                        // draw grid if enabled <- This is really a debug feature
                        // ==========================================================================================================================
                        if (DebugGUI::guiValues.drawGridOverTexture)
                        {
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderDrawRectF(renderer, &destRect);

                            if (tileX == x && tileY == y)
                            {
                                // We wanna highlight the current tile red 
                                // by red rect and opacity 50
                                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50);
                                SDL_RenderFillRectF(renderer, &destRect);

                                int tempIndex = tileIndex + offset; 
                                DebugGUI::showSelectedSDLTexture(
                                    texture,
                                    (tileIndex % maxColumns) * tileMap->tileWidth,   
                                    (tileIndex / maxColumns) * tileMap->tileHeight,  
                                    tileMap->tileWidth,
                                    tileMap->tileHeight
                                );
                            }
                        }
                        // ==========================================================================================================================
                        // End of Drawing
                        // ==========================================================================================================================
                    }
                }
            }
        }
        // ==========================================================================================================================
        // End Of Function
        // ==========================================================================================================================
        return true;
    }
