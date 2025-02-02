#include "shared/game.h"
#include "configs.h"
#include <SDL2/SDL_render.h>
#include <SDL_hints.h>
#include <fstream>
#include <string>

Game::Game() 
{
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

    int windowWidth = WIDTH; 
    int windowHeight = HEIGHT;
    // Calculate camera offset based on the player’s position
    // Basically is the center of the player
    int camX = player->getX() + player->getWidth() / 2 - windowWidth / 2;
    int camY = player->getY() + player->getHeight() / 2 - windowHeight / 2;

    // Apply camera offset when drawing the player
    playerRect.x = player->getX() - camX;
    playerRect.y = player->getY() - camY;
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
    // Set a different color for remote players
    this->drawGreen();

    // Calculate camera offset based on the local player's position
    Player* localPlayer = this->client->getPlayer();
    int windowWidth = WIDTH;
    int windowHeight = HEIGHT;
    int camX = localPlayer->getX() + localPlayer->getWidth() / 2 - windowWidth / 2;
    int camY = localPlayer->getY() + localPlayer->getHeight() / 2 - windowHeight / 2;

    for (Player* remotePlayer : remotePlayers)
    {
        if (!remotePlayer) { continue; }
        SDL_Rect remotePlayerRect;
        // Placing The Remote Players in a place relative to where the person
        // is in the game not the screen
        remotePlayerRect.x = remotePlayer->getX() - camX;
        remotePlayerRect.y = remotePlayer->getY() - camY;
        remotePlayerRect.w = remotePlayer->getWidth();
        remotePlayerRect.h = remotePlayer->getHeight();
        SDL_RenderFillRect(renderer, &remotePlayerRect);
    }
}

void Game::start_game()
{
    this->client->handlePlayerJoined(this->client->getPlayer()->getName());

    // Set The Player X and Y Coordinate To The Center of The Screen
    this->client->getPlayer()->setX(static_cast<int>(WIDTH) / 2);
    this->client->getPlayer()->setY(static_cast<int>(HEIGHT) / 2);

    float oldX = this->client->getPlayer()->getX();
    float oldY = this->client->getPlayer()->getY();

    // Load Map
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

    const int displayTileSize = 48;

    int mapWidthInTiles = (mapData.size() > 0 ? mapData[0].size() : 0);
    int mapHeightInTiles = mapData.size();

    int mapPixelWidth = mapWidthInTiles * displayTileSize;
    int mapPixelHeight = mapHeightInTiles * displayTileSize;

    // Here, instead of centering the map statically,
    // we use the camera offset calculated from the player's position.
    Player* player = this->client->getPlayer();
    int windowWidth = WIDTH;
    int windowHeight = HEIGHT;
    int camX = player->getX() + player->getWidth() / 2 - windowWidth / 2;
    int camY = player->getY() + player->getHeight() / 2 - windowHeight / 2;

    for (int y = 0; y < mapData.size(); y++)
    {
        for (int x = 0; x < mapData[y].size(); x++)
        {
            int tileIndex = mapData[y][x] - 1;
            if (tileIndex >= 0 && tileIndex < tiles.size())
            {
                SDL_Rect destRect = {
                    x * displayTileSize - camX,  // Apply camera offset here
                    y * displayTileSize - camY,
                    displayTileSize,
                    displayTileSize
                };
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

// Temp Data
std::vector<int> row1 = {
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
};
/** 
 * This File Will Contain The Map Data
 * The Map Data will have a value from
 * 0x00 - 0xFF
 **/
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
    while (std::getline(file, line))
    {
        std::vector<int> row;
        std::istringstream iss(line);
        std::string cell;
        
        // Read each hex value (separated by whitespace)
        while (iss >> cell)
        {
            try {
                // Convert the hex string to an integer
                int cellValue = std::stoi(cell, nullptr, 16);
                row.push_back(cellValue);
            }
            catch(const std::exception &e) {
                std::cerr << "Error converting '" << cell << "': " << e.what() << std::endl;
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
