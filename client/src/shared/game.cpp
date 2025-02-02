#include "shared/game.h"
#include <SDL2/SDL_render.h>
#include <SDL_hints.h>
#include <fstream>
#include <string>

Game::Game() {
    this->client = new Client();
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to Initialize SDL2 Library" << std::endl;
        return;
    }
    this->initWindow();
    this->initRenderer();
    this->keep_window_open = true;
}

void Game::renderPlayer()
{
    SDL_Rect playerRect;
    Player* player = this->client->getPlayer();
    playerRect.x = player->getX();
    playerRect.y = player->getY();
    playerRect.w = player->getWidth();
    playerRect.h = player->getHeight();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerRect);
}

void Game::drawGreen() { SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255); }
void Game::drawBlue() { SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255); }
void Game::drawRed() { SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255); }
void Game::drawYellow() { SDL_SetRenderDrawColor(this->renderer, 255, 255, 0, 255); }


void Game::renderOtherPlayers()
{
    const auto& remotePlayers = this->client->getPlayersSafe();
    // Set Different Color For Remote Players
    this->drawGreen();

    for (Player* remotePlayer : remotePlayers)
    {
        if (!remotePlayer) { continue; }

        SDL_Rect remotePlayerRect;
        remotePlayerRect.x = remotePlayer->getX();
        remotePlayerRect.y = remotePlayer->getY();
        remotePlayerRect.w = remotePlayer->getWidth();
        remotePlayerRect.h = remotePlayer->getHeight();
        SDL_RenderFillRect(renderer, &remotePlayerRect);
    }
}

void Game::start_game()
{
    this->client->handlePlayerJoined(this->client->getPlayer()->getName());
    float oldX = this->client->getPlayer()->getX();
    float oldY = this->client->getPlayer()->getY();
    
    std::string currentPath = __FILE__;
    currentPath = currentPath.substr(0, currentPath.find_last_of("/\\") + 1);
    
    // Load Tileset Atlas
    this->loadTileset(currentPath + "../../Assets/tile_atlas.png");
    // Load Map
    this->parseMap(currentPath + "../../Maps/level_1.txt");

    while(this->keep_window_open)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e)){ this->handleEvent(e); }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        this->renderMap();
        
        // Constanty send Player Position to Server
        this->updateServer(&oldX,&oldY);

        this->renderPlayer();
        this->renderOtherPlayers();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Game::renderMap()
{
    int srcTileSize = 15;

    const int displayTileSize = 32;

    int mapWidthInTiles = (mapData.size() > 0 ? mapData[0].size() : 0);
    int mapHeightInTiles = mapData.size();

    int mapPixelWidth = mapWidthInTiles * displayTileSize;
    int mapPixelHeight = mapHeightInTiles * displayTileSize;

    // Get Window Dimensions
    int windowWidth = 800;
    int windowHeight = 600;

    // Center the Map
    int offsetX = (windowWidth - mapPixelWidth) / 2;
    int offsetY = (windowHeight - mapPixelHeight) / 2;

    for (int y = 0; y < mapData.size(); y++)
    {
        for (int x = 0; x < mapData[y].size(); x++)
        {
            int tileIndex = mapData[y][x] - 1;
            if (tileIndex >= 0 && tileIndex < tiles.size())
            {
                SDL_Rect destRect = {
                    offsetX + x * displayTileSize, 
                    offsetY + y * displayTileSize, 
                    displayTileSize, displayTileSize};
                SDL_RenderCopy(this->renderer, this->tileAtlasTexture, &this->tiles[tileIndex], &destRect);
            }
        }
    }
}

void Game::loadTileset(std::string filePath)
{
    SDL_Surface* tileAtlasSurface = IMG_Load(filePath.c_str());
    if (!tileAtlasSurface)
    {
        std::cout << "Failed to Load Tileset" << std::endl;
        return;
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    this->tileAtlasTexture = SDL_CreateTextureFromSurface(this->renderer, tileAtlasSurface);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_FreeSurface(tileAtlasSurface);

    if (!this->tileAtlasTexture)
    {
        std::cout << "Failed to Create Texture from Surface" << std::endl;
        return;
    }
    this->initializeTiles();
}

/** 
 * Atlas is 208x208
 * with 16x16 tiles
 **/
void Game::initializeTiles() {
    int tileSize = 15;
    int atlasWidth = 208;
    int atlasHeight = 208;

    int padding = 0; // Set to 0 if there's no spacing

    int tilesPerRow = atlasWidth / (tileSize + padding);
    int tilesPerCol = atlasHeight / (tileSize + padding);

    for (int y = 0; y < tilesPerCol; y++) {
        for (int x = 0; x < tilesPerRow; x++) {
            SDL_Rect tileRect {
                x * (tileSize + padding),  // Calculate actual pixel position
                y * (tileSize + padding),
                tileSize,
                tileSize
            };
            this->tiles.push_back(tileRect);
        }
    }
}

void Game::parseMap(std::string path)
{
    std::cout << "File Path: " << path << std::endl;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Failed to open file" << std::endl;
        return;
    }
    std::string line;
    while(std::getline(file, line))
    {
        std::vector<int> row;
        for (char c: line)
        {
            if (c >= '0' && c <= '9')
            {
                row.push_back(c - '0');
            } else {
                row.push_back(-1);
            }
        }
        this->mapData.push_back(row);
    }
    file.close();
}

void Game::updateServer(float *oldX, float *oldY)
{
    Player* player = this->client->getPlayer();
    // Only Call This if the Player has Moved
    if(*oldX != player->getX() || *oldY != player->getY()) {
        this->client->handlePlayerMoved(player->getID(), player->getX(), player->getY());
        *oldX = player->getX();
        *oldY = player->getY();
    }
}


void Game::handleEvent(SDL_Event e)
{
    Player *player = this->client->getPlayer();
    switch (e.type)
    {
        case SDL_QUIT:
            this->keep_window_open = false;
            break;
    }
    switch (e.key.keysym.sym) {
        case SDLK_w:
            std::cout << "w pressed" << std::endl;
            // Move player up
            player->setY(player->getY() - player->getSpeed());
            break;
        case SDLK_a:
            std::cout << "a pressed" << std::endl;
            // Move player left
            player->setX(player->getX() - player->getSpeed());
            break;
        case SDLK_s:
            std::cout << "s pressed" << std::endl;
            // Move player down
            player->setY(player->getY() + player->getSpeed());
            break;
        case SDLK_d:
            std::cout << "d pressed" << std::endl;
            // Move player right
            player->setX(player->getX() + player->getSpeed());
            break;
    }
}


void Game::initWindow()
{
    this->window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!this->window)
    {
        printf("Failed to make window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
}

void Game::initRenderer()
{
    this->renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if(!this->renderer)
    {
        printf("Failed To Get the surface from the window");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
}
