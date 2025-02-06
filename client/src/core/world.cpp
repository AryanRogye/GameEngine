#include "world.h"
#include "configs.h"
#include <SDL_blendmode.h>
#include <SDL_keycode.h>
#include <SDL_render.h>

World::World(
    Sprite playerIdleSprite, 
    Sprite playerRunSprite, 
    SDL_Renderer* renderer, 
    SDL_Window* window
)
: playerIdleSprite(playerIdleSprite), playerRunSprite(playerRunSprite)
{
    this->window = window;      /** Set Window **/
    this->renderer = renderer;  /** Set Renderer **/
    this->enterHouse = false;   /** Make Sure House is not enterable at start **/

    this->currentPath = __FILE__;
    this->currentPath = currentPath.substr(0, currentPath.find_last_of("/\\") + 1);

    /** Block Factory **/
    this->bf = BlockFactory();

    /** Client Connection **/
    this->client = std::make_unique<Client>();
    /** Set A MapLoader **/
    this->mapLoader = MapLoader(this->currentPath + "../../Maps/level_1.txt");
    CommandSystem::getInstance().setupCommands();
}

void World::updateAndRender()
{
    /** Get initial player position cuz later on we check old
     *  positions for optimizations or whatever we need it for
     **/
    float oldX = this->client->getPlayer()->getX();
    float oldY = this->client->getPlayer()->getY();
    while(this->keep_window_open)
    {
        SDL_Event e; while(SDL_PollEvent(&e)){ this->handleEvent(e); }

        mapLoader.hotReload(this->mapData);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);
        
        // Render Map
        this->renderMap();
        this->updateServer(&oldX, &oldY);

        this->renderPlayer();
        this->renderOtherPlayers();
        this->checkIfPosIsEnterable();
        // Kinda Like Minecraft Black Box Chat thing
        this->renderCommandInput();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }
}


void World::handleEvent(SDL_Event e) {
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
        
        // Regular Movement For The Player
        if (!this->commandMode)
        {
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
                    break;
            }
        }
        // Things Going on in the Command Mode
        else
        {
            // If Enter Is Pressed in Command Mode
            if (e.key.keysym.sym == SDLK_RETURN)
            {
                std::cout << "Command Entered: " << this->commandInput << std::endl;
                // Execute The Command
                CommandSystem::getInstance().executeCommand(this->commandInput, this);
                // Reset The Command Input and Turn off Command Mode
                this->commandInput.clear();
                this->commandMode = false;
            }
            // If Backspace is Pressed in Command Mode
            else if (e.key.keysym.sym == SDLK_BACKSPACE)
            {
                // Dont Want to remove nothing lol
                if (!this->commandInput.empty())
                {
                    this->commandInput.pop_back();
                }
            }
            // Turn off Command Mode WIth Escape
            else if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                this->commandInput.clear();
                this->commandMode = false;
            } else {
                // Allow only printable characters (a-z, A-Z, 0-9, symbols)
                if ((e.key.keysym.sym >= SDLK_SPACE && e.key.keysym.sym <= SDLK_z) ||
                    (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9)) {
                    this->commandInput += static_cast<char>(e.key.keysym.sym);
                }
            }
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

        /** Command Stuff **/
        switch (e.key.keysym.sym) {
            case SDLK_SLASH:
                this->commandMode = !this->commandMode;
                this->commandInput += '/';
                break;
        }
    }
    else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) {
            player->setIsWalking(false);
        }
    }
}

/** 
 * Main Loop of the Game Will Be Called in Here
 * **/
void World::setupWorld()
{
    /** Start The Client **/
    this->client->handlePlayerJoined(this->client->getPlayer()->getName());

    /** Set The Player X and Y Coordinate To The Center of The Screen **/
    this->client->getPlayer()->setX(static_cast<int>(WIDTH / 2));
    this->client->getPlayer()->setY(static_cast<int>(HEIGHT / 2));    

    // Load Tiles
    if(!Texture::loadTexture(
        currentPath + "../../Assets/tile_atlas.png",
        &this->tileAtlasTexture,
        this->renderer
    )) std::cout << "Failed to load tileAtlasTexture" << std::endl;
    // Initialize Empty Rect Tiles
    Sprite::fillRectVector(this->tiles, 208, 208, 16);

    // Load The Map
    this->mapLoader.parseFile(this->mapData);

    // Load House Texture
    if(!Texture::loadTexture(
        currentPath + "../../Assets/Houses/house_1.png",
        &this->house_one_texture,
        this->renderer
    )) std::cout << "Failed to load houseTexture" << std::endl;

    /** 
     * Load Textures Of Player
     *      - Idle
     *      - Run
     **/
    if(!Texture::loadTexture(
        this->playerIdleSprite.getPath(),
        &this->playerIdleTexture,
        this->renderer
    )) std::cout << "Failed to load playerIdleTexture" << std::endl;

    if(!Texture::loadTexture(
        this->playerRunSprite.getPath(),
        &this->playerRunTexture,
        this->renderer
    )) std::cout << "Failed to load playerRunTexture" << std::endl;

    // Load The Font
    this->loadFont();
}

void World::loadFont()
{
    /** Font Is Just a Sprite Sheet so we can just use the sprite class **/
    Sprite::fillRectVector(this->fonts,128,128, 8,7);
    /** Load Into the texture **/
    Texture::loadTexture(this->currentPath + "../../Assets/font.png", &this->font_texture, this->renderer);
}

void World::renderMap()
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
    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - windowWidth / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - windowHeight / 2);

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

void World::renderHouse(int x, int y, int width, int height)
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

void World::updateServer(float *oldX, float *oldY)
{
    Player* player = this->client->getPlayer();
    // Only Call This if the Player has Moved
    if(*oldX != player->getX() || *oldY != player->getY()) {
        this->client->handlePlayerMoved(player->getID(), player->getX(), player->getY());
        *oldX = player->getX();
        *oldY = player->getY();
    }

    if (this->mapData.size() > 0)
    {
        // I want to send The Max Rows and Cols inside Here
        // The Reason Why is cuz the server will send the palyer where the zombies will spawn
        /*this->client->handleZombieSpawn(this->mapData.size(), this->mapData[0].size());*/
    }
}

void World::renderPlayer()
{
    Player* player = this->client->getPlayer();
    Uint32 now = SDL_GetTicks();
    int feetY = player->getY() + player->getHeight();
    int centerX = player->getX() + static_cast<int>(player->getWidth() / 2);

    if (!player->getIsWalking())
    {
        // First Thing Reset the Running Frame
        this->playerRunSprite.setCurrentFrame(0);
        if (now - this->lastFrameTime >= this->frameDelay)
        {
            this->playerIdleSprite.setCurrentFrame((this->playerIdleSprite.getCurrentFrame() + 1) % this->playerIdleSprite.getFrameCount());
            this->lastFrameTime = now;
        }
        Sprite::renderSprite(this->playerIdleSprite, this->renderer, playerIdleTexture, 30, 45, player);
        this->bf.printBlockInfoByPosition(centerX, feetY, this->mapData);
    }
    else
    {
        // First Thing Reset the Idle Frame
        this->playerIdleSprite.setCurrentFrame(0);
        if (now - this->lastFrameTime >= this->frameDelay)
        {
            this->playerRunSprite.setCurrentFrame ( (this->playerRunSprite.getCurrentFrame() + 1) % this->playerRunSprite.getFrameCount());
            this->lastFrameTime = now;
        }
        Sprite::renderSprite(this->playerRunSprite, this->renderer, this->playerRunTexture, 30, 45, player);
        this->bf.printBlockInfoByPosition(centerX, feetY, this->mapData);
    }
}

void World::drawGreen() { SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255); }
void World::drawBlue() { SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255); }
void World::drawRed() { SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255); }
void World::drawYellow() { SDL_SetRenderDrawColor(this->renderer, 255, 255, 0, 255); }

void World::renderOtherPlayers()
{
    const auto& remotePlayers = this->client->getPlayersSafe();
    // Set a different color for remote players
    this->drawGreen();

    // Calculate camera offset based on the local player's position
    Player* localPlayer = this->client->getPlayer();
    int windowWidth = WIDTH;
    int windowHeight = HEIGHT;
    int camX = localPlayer->getX() + static_cast<int>(localPlayer->getWidth() / 2 - windowWidth / 2);
    int camY = localPlayer->getY() + static_cast<int>(localPlayer->getHeight() / 2 - windowHeight / 2);

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

// Transparent Skinny Box Stetching across the screen
void World::renderCommandInput()
{
    if (this->commandMode)
    {
        this->commandBox.x = this->command_box_x;
        this->commandBox.y = this->command_box_y;
        this->commandBox.w = this->command_box_width;
        this->commandBox.h = this->command_box_height;

        SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(this->renderer, &this->commandBox);

        if(!this->commandInput.empty())
        {
            UI::renderTextAtPosition(
                this->renderer,this->font_texture, this->fonts, this->commandInput, 5,
                this->command_box_y + 5, FONT_WIDTH, FONT_HEIGHT, FONT_SCALE - 1, false, 1
            );
        }
    }
}
Client* World::getClient() { return client.get(); }
void World::checkIfPosIsEnterable()
{
    if(this->bf.checkEnterable(this->client->getPlayer()->getX(), this->client->getPlayer()->getY(), this->mapData))
    {
        this->enterHouse = true;
        // Render Text To Screen That tells the person "oh you can enter"
        UI::renderTextAtPosition(this->renderer, this->font_texture, this->fonts, "Press [E] to Enter", 20, 20, FONT_WIDTH, FONT_HEIGHT, FONT_SCALE, false, 1);
    }
    else
    {
        this->enterHouse = false;
    }
}
