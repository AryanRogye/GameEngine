#include "shared/game.h"
#include "configs.h"
#include <cstdio>

Game::Game() 
{
    this->enterHouse = false;
    this->bf = BlockFactory();
    this->currentIdleFrame = 0;
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

void Game::loadPlayerSprites(std::string filePath)
{
    // Player Character
    SDL_Surface* playerSurface = IMG_Load((filePath + "../../Assets/char_idle2.png").c_str());
    if (!playerSurface)
    {
        std::cout << "Failed to Load Player Sprite" << std::endl;
        return;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    this->playerTexture = SDL_CreateTextureFromSurface(this->renderer, playerSurface);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_FreeSurface(playerSurface);
    if (!this->playerTexture)
    {
        std::cout << "Failed to Create Texture from Surface" << std::endl;
        return;
    }
    // Running Sprite Loading
    SDL_Surface* runningSurface = IMG_Load((filePath + "../../Assets/char_run1.png").c_str());
    if (!runningSurface)
    {
        std::cout << "Failed to Load Running Sprite" << std::endl;
        return;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    this->runningTexture = SDL_CreateTextureFromSurface(this->renderer, runningSurface);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_FreeSurface(runningSurface);
    if (!this->runningTexture)
    {
        std::cout << "Failed to Create Texture from Surface" << std::endl;
        return;
    }
}
void Game::renderPlayer()
{
    Player* player = this->client->getPlayer();

    int windowWidth = WIDTH; 
    int windowHeight = HEIGHT;
    // Calculate camera offset based on the player’s position
    // Basically is the center of the player
    int camX = player->getX() + player->getWidth() / 2 - windowWidth / 2;
    int camY = player->getY() + player->getHeight() / 2 - windowHeight / 2;

    Uint32 now = SDL_GetTicks();

    if (!player->getIsWalking())
    {
        // First Thing Reset the Running Frame
        this->currentRunningFrame = 0;
        if (now - this->lastFrameTime >= this->frameDelay)
        {
            this->currentIdleFrame = (this->currentIdleFrame + 1) % 4;
            this->lastFrameTime = now;
        }
        // Define the source rectangle if using a sprite sheet (assuming each sprite is 30x30)
        SDL_Rect srcRect = { this->currentIdleFrame * 30, 0, 30, 45 };
        // Destination rectangle based on your camera offset calculations
        SDL_Rect destRect = { (int)player->getX() - camX, (int)player->getY() - camY, PLAYER_WIDTH, PLAYER_HEIGHT};
        // Determine if the sprite should be flipped horizontally
        SDL_RendererFlip flip = player->getFacingRight() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        // Render the sprite with the flip
        SDL_RenderCopyEx(renderer, playerTexture, &srcRect, &destRect, 0.0, NULL, flip);
        // Printing Block Info By Posttion we need to get the feet of the character
        int feetY = player->getY() + player->getHeight();
        int centerX = player->getX() + player->getWidth() / 2;
        
        this->bf.printBlockInfoByPosition(centerX, feetY, this->mapData);
    }
    else
    {
        // First Thing Reset the Idle Frame
        this->currentIdleFrame = 0;
        if (now - this->lastFrameTime >= this->frameDelay)
        {
            this->currentRunningFrame = (this->currentRunningFrame + 1) % 4;
            this->lastFrameTime = now;
        }
        // Define the source rectangle if using a sprite sheet (assuming each sprite is 30x30)
        SDL_Rect srcRect = { this->currentRunningFrame * 30, 0, 30, 30 };
        // Destination rectangle based on your camera offset calculations
        SDL_Rect destRect = { (int)player->getX() - camX, (int)player->getY() - camY, TILE_SIZE, TILE_SIZE};
        // Determine if the sprite should be flipped horizontally
        SDL_RendererFlip flip = player->getFacingRight() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        // Render the sprite with the flip
        SDL_RenderCopyEx(renderer, runningTexture, &srcRect, &destRect, 0.0, NULL, flip);
        int feetY = player->getY() + player->getHeight();
        int centerX = player->getX() + player->getWidth() / 2;
        this->bf.printBlockInfoByPosition(centerX, feetY, this->mapData);
    }
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
    this->loadTileset(currentPath);

    // Load Map Initially
    MapLoader mapLoader = MapLoader(currentPath + "../../Maps/level_1.txt");
    mapLoader.parseFile(this->mapData);

    // Load The House Texture
    this->loadHouseSprites(currentPath);

    this->loadPlayerSprites(currentPath);

    while(this->keep_window_open)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e)){ this->handleEvent(e); }

        mapLoader.hotReload(this->mapData);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        this->renderMap();
        
        // Constanty send Player Position to Server
        this->updateServer(&oldX,&oldY);

        this->renderPlayer();
        this->renderOtherPlayers();
        this->checkIfPosIsEnterable();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Game::renderMap()
{
    int srcTileSize = 16;

    const int displayTileSize = TILE_SIZE;

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
    
    std::vector<std::vector<bool>> visited(mapHeightInTiles, std::vector<bool>(mapWidthInTiles, false));
    for (int y = 0; y < mapData.size(); y++)
    {
        for (int x = 0; x < mapData[y].size(); x++)
        {
            if (visited[y][x]) { continue; }
            int tileIndex = mapData[y][x] - 1;

            // Blank Tile
            if (tileIndex + 1 == 0) {
                // Draw Empty Space
                SDL_Rect destRect = {
                    x * displayTileSize - camX,  // Apply camera offset here
                    y * displayTileSize - camY,
                    displayTileSize,
                    displayTileSize
                };
            }
            // House Tile
            if (tileIndex + 1 == -1) {
                // Render The House1 Texture
                this->renderHouse(x * displayTileSize - camX, y * displayTileSize - camY, displayTileSize, displayTileSize);
                // We Want to set the x + 1, y + 1, x + 1 y + 1 to visited
                if (y + 1 < mapHeightInTiles) visited[y + 1][x] = true;
                if (x + 1 < mapWidthInTiles) visited[y][x + 1] = true;
                if (y + 1 < mapHeightInTiles && x + 1 < mapWidthInTiles) visited[y + 1][x + 1] = true;
                continue;
            }
            if (tileIndex >= 0 && tileIndex < tiles.size())
            {
                SDL_Rect destRect = {
                    x * displayTileSize - camX,  // Apply camera offset here
                    y * displayTileSize - camY,
                    displayTileSize,
                    displayTileSize
                };
                visited[y][x] = true;
                SDL_RenderCopy(this->renderer, this->tileAtlasTexture, &this->tiles[tileIndex], &destRect);
            }
        }
    }
}

void Game::renderHouse(int x, int y, int width, int height)
{
    // Size is 30x30 we can scale it to what the width and height is
    SDL_Rect destRect = {
        x,
        y,
        width * 2,
        height * 2
    };
    SDL_RenderCopy(this->renderer, this->house_one_texture, NULL, &destRect);

}
void Game::loadHouseSprites(std::string filePath)
{
    SDL_Surface* houseSurface = IMG_Load((filePath + "../../Assets/Houses/house_1.png").c_str());
    if (!houseSurface)
    {
        std::cout << "Failed to Load House Sprite" << std::endl;
        return;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    this->house_one_texture = SDL_CreateTextureFromSurface(this->renderer, houseSurface);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_FreeSurface(houseSurface);
    if (!this->house_one_texture)
    {
        std::cout << "Failed to Create Texture from Surface" << std::endl;
        return;
    }
}

void Game::loadTileset(std::string filePath)
{
    SDL_Surface* tileAtlasSurface = IMG_Load((filePath + "../../Assets/tile_atlas.png").c_str());
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
    int tileSize = 16;
    int atlasWidth = 208;
    int atlasHeight = 208;

    int padding = 0; // Set to 0 if there's no spacing

    int tilesPerRow = atlasWidth / (tileSize + padding);
    int tilesPerCol = atlasHeight / (tileSize + padding);

    std::cout << "Tiles Per Row: " << tilesPerRow << std::endl;
    std::cout << "Tiles Per Col: " << tilesPerCol << std::endl;

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


void Game::handleEvent(SDL_Event e) {
    Player *player = this->client->getPlayer();

    if (e.type == SDL_QUIT) {
        this->keep_window_open = false;
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        int newX = player->getX();
        int newY = player->getY();
        int speed = player->getSpeed();


        switch (e.key.keysym.sym) {
            case SDLK_w:
                newY -= speed;
                break;
            case SDLK_a:
                newX -= speed;
                player->setFacingRight(false);
                player->setIsWalking(true);
                break;
            case SDLK_s:
                newY += speed;
                break;
            case SDLK_d:
                newX += speed;
                player->setFacingRight(true);
                player->setIsWalking(true);
                break;
            case SDLK_e:
                if (this->enterHouse)
                    std::cout << "E Key Pressed (Allowed)" << std::endl;
                else
                    std::cout << "E Key Pressed (Not Allowed)" << std::endl;
        }

        // Check if the new position is valid
        int feetY = newY + player->getHeight() + 1;
        int centerX = newX + player->getWidth() / 2;

        bool isColliding = this->bf.checkCollision(centerX, feetY, this->mapData);

        // Add Enterable Here
        if (!isColliding) 
        {
            player->setX(newX);
            player->setY(newY);
        }
        else 
        {
            /** 
             * Dont Really Have to do anything cuz this means that the 
             * Player cannot move at all im gonna keep the else block
             * so that its easier to understan what is happening
             **/
        }
    }
    else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) {
            player->setIsWalking(false);
        }
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

void Game::checkIfPosIsEnterable()
{
    if(this->bf.checkEnterable(this->client->getPlayer()->getX(), this->client->getPlayer()->getY(), this->mapData))
    {
        this->enterHouse = true;
    }
    else
    {
        this->enterHouse = false;
    }
}
