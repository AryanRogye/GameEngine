#include "world.h"

World::World(
    SDL_Renderer* renderer,
    SDL_Window* window
)
:   
    playerFrontIdleSprite(PlayerFrontIdlePaths[spriteIndex]), 
    playerBackIdleSprite(PlayerBackIdlePaths[spriteIndex]), 
    playerSideIdleSprite(PlayerSideIdlePaths[spriteIndex]),
    playerFrontRunSprite(PlayerFrontRunningPaths[spriteIndex]), 
    playerBackRunSprite(PlayerBackRunningPaths[spriteIndex]), 
    playerSideRunSprite(PlayerSideRunningPaths[spriteIndex])
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

    Uint32 lastTime = SDL_GetTicks();
    while(this->keep_window_open)
    {
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        SDL_Event e; while(SDL_PollEvent(&e)){ this->handleEvent(e); }

        mapLoader.hotReload(this->mapData);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);

        // Render Map
        this->renderMap();
        this->updateServer(&oldX, &oldY);

        this->renderPlayer();
        this->renderOtherPlayers();
        this->checkIfPosIsEnterable();
        this->checkIfPosIsBreakable();
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        handlePlayerMovement(state, dt);
        // Kinda Like Minecraft Black Box Chat thing
        this->renderCommandInput();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }
}


void World::handlePlayerMovement(const Uint8* state, float dt) 
{
    if (!this->keep_window_open) return;
    if (this->commandMode) return;

    Player *player = this->client->getPlayer();
    float speed = player->getSpeed();

    float dx = 0.0f;
    float dy = 0.0f;

    // Track which directions are being pressed
    bool pressedW = state[SDL_SCANCODE_W];
    bool pressedS = state[SDL_SCANCODE_S];
    bool pressedA = state[SDL_SCANCODE_A];
    bool pressedD = state[SDL_SCANCODE_D];

    // Set movement vector
    if (pressedW) dy -= 1.0f;
    if (pressedA) dx -= 1.0f;
    if (pressedS) dy += 1.0f;
    if (pressedD) dx += 1.0f;

    // Set facing direction with priority
    if (pressedS && !pressedW) player->setFacing(Facing::FORWARD);
    else if (pressedW && !pressedS) player->setFacing(Facing::BACKWARD);
    else if (pressedA && !pressedD) player->setFacing(Facing::LEFT);
    else if (pressedD && !pressedA) player->setFacing(Facing::RIGHT);
    // If no direction is pressed, keep the last facing direction

    /*std::cout << "Current Facing : " << player->getFacing() << std::endl;*/

    // Normalize the vector to ensure diagonal movement isn't faster
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0.0f) {
        dx /= length;
        dy /= length;
    }

    // Apply movement
    float newX = player->getX() + dx * speed * dt;
    float newY = player->getY() + dy * speed * dt;
    bool moving = (dx != 0.0f || dy != 0.0f);
    player->setIsWalking(moving);

    // Collision Handling
    int hitboxOffsetX = 0;
    int hitboxOffsetY = 0;

    // Try horizontal movement
    float originalX = player->getX();
    player->setX(newX);
    player->updateWorldHitbox(hitboxOffsetX, hitboxOffsetY);
    if (this->bf.checkBlockState(player->getWorldHitbox(), this->mapData) & BlockState::COLLISION) {
        player->setX(originalX);  // Undo horizontal movement
    }

    // Try vertical movement
    float originalY = player->getY();
    player->setY(newY);
    player->updateWorldHitbox(hitboxOffsetX, hitboxOffsetY);
    if (this->bf.checkBlockState(player->getWorldHitbox(), this->mapData) & BlockState::COLLISION) {
        player->setY(originalY);  // Undo vertical movement
    }
}

void World::handleCommandInput(SDL_Event& e) {
    if (e.key.keysym.sym == SDLK_RETURN) {
        std::cout << "Command Entered: " << this->commandInput << std::endl;
        CommandSystem::getInstance().executeCommand(this->commandInput, this);
        this->commandInput.clear();
        this->commandMode = false;
    }
    else if (e.key.keysym.sym == SDLK_BACKSPACE) {
        if (!this->commandInput.empty()) {
            this->commandInput.pop_back();
        }
    }
    else if (e.key.keysym.sym == SDLK_ESCAPE) {
        this->commandInput.clear();
        this->commandMode = false;
    }
    else {
        if ((e.key.keysym.sym >= SDLK_SPACE && e.key.keysym.sym <= SDLK_z) ||
            (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9)) {
            this->commandInput += static_cast<char>(e.key.keysym.sym);
        }
    }
}

void World::handleEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT) {
        this->keep_window_open = false;
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        keyStates[e.key.keysym.sym] = true;  // ✅ Mark key as pressed

        if (this->enterHouse)
        {
            if (e.key.keysym.sym == SDLK_e)
            {
                // Right now we have this but later we need to allow camera change
                std::cout << "Press E To Enter" << std::endl;
            }
        }

        if (this->allowBreak)
        {
            if (e.key.keysym.sym == SDLK_e)
            {
                std::cout << "Press E To Break" << std::endl;
            }
        }

        if (this->commandMode)
        {
            // ✅ Moved command input here!
            handleCommandInput(e);
        }
        if (e.key.keysym.sym == SDLK_SLASH)
        {
            this->commandMode = !this->commandMode;
            this->commandInput += '/';
        }
    }
    else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_w || 
            e.key.keysym.sym == SDLK_a || 
            e.key.keysym.sym == SDLK_s || 
            e.key.keysym.sym == SDLK_d) {
            Player *player = this->client->getPlayer();
            player->setIsWalking(false);
        }
    }
}

/**
 * Main Loop of the Game Will Be Called in Here
 * **/
void World::setupWorld()
{
    /*// Get the screen dimensions*/
    /*SDL_DisplayMode DM;*/
    /*SDL_GetCurrentDisplayMode(0, &DM);  // Get primary display resolution*/
    /**/
    /*this->screenWidth = DM.w;*/
    /*this->screenHeight = DM.h;*/
    /**/
    /*// Resize the window to match screen dimensions*/
    /*SDL_SetWindowSize(this->window, screenWidth, screenHeight);*/
    /*SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);*/

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
    // Get The Size of the Tile Atlas
    int tileAtlasWidth, tileAtlasHeight;
    SDL_QueryTexture(this->tileAtlasTexture, NULL, NULL, &tileAtlasWidth, &tileAtlasHeight);
    Sprite::fillRectVector(this->tiles, tileAtlasWidth, tileAtlasHeight, 16);

    // Load The Map
    this->mapLoader.parseFile(this->mapData);


    /**
     * Load Textures Of Player
     *      - Idle
     *      - Run
     **/
    if(!Texture::loadTexture(
        this->playerFrontIdleSprite.getPath(),
        &this->playerFrontIdleTexture,
        this->renderer
    )) std::cout << "Failed to load playerIdleTexture" << std::endl;
    if(!Texture::loadTexture(
        this->playerBackIdleSprite.getPath(),
        &this->playerBackIdleTexture,
        this->renderer
    )) std::cout << "Failed to load playerIdleTexture" << std::endl;
    if(!Texture::loadTexture(
        this->playerSideIdleSprite.getPath(),
        &this->playerSideIdleTexture,
        this->renderer
    )) std::cout << "Failed to load playerIdleTexture" << std::endl;
    
    if(!Texture::loadTexture(
        this->playerFrontRunSprite.getPath(),
        &this->playerFrontRunTexture,
        this->renderer
    )) std::cout << "Failed to load playerRunTexture" << std::endl;
    if(!Texture::loadTexture(
        this->playerBackRunSprite.getPath(),
        &this->playerBackRunTexture,
        this->renderer
    )) std::cout << "Failed to load playerRunTexture" << std::endl;
    if(!Texture::loadTexture(
        this->playerSideRunSprite.getPath(),
        &this->playerSideRunTexture,
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
    const int BORDER_SIZE = 48;
    const int TILE_BUFFER = 2; // Add extra tiles to prevent pop-in
    
    int srcTileSize = 16;
    const int displayTileSize = TILE_SIZE;
    int mapWidthInTiles = (mapData.size() > 0 ? mapData[0].size() : 0);
    int mapHeightInTiles = mapData.size();
    Player* player = this->client->getPlayer();

    // Camera follows player, centered
    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - (WIDTH - 2 * BORDER_SIZE) / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - (HEIGHT - 2 * BORDER_SIZE) / 2);

    // Calculate visible area with buffer
    int startX = std::max(0, (camX - BORDER_SIZE) / displayTileSize - TILE_BUFFER);
    int startY = std::max(0, (camY - BORDER_SIZE) / displayTileSize - TILE_BUFFER);
    int endX = std::min(mapWidthInTiles, (camX + WIDTH) / displayTileSize + TILE_BUFFER);
    int endY = std::min(mapHeightInTiles, (camY + HEIGHT) / displayTileSize + TILE_BUFFER);

    // Render black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    // Top border
    SDL_Rect topBorder = {0, 0, WIDTH, BORDER_SIZE};
    SDL_RenderFillRect(renderer, &topBorder);
    
    // Bottom border
    SDL_Rect bottomBorder = {0, HEIGHT - BORDER_SIZE, WIDTH, BORDER_SIZE};
    SDL_RenderFillRect(renderer, &bottomBorder);
    
    // Left border
    SDL_Rect leftBorder = {0, 0, BORDER_SIZE, HEIGHT};
    SDL_RenderFillRect(renderer, &leftBorder);
    
    // Right border
    SDL_Rect rightBorder = {WIDTH - BORDER_SIZE, 0, BORDER_SIZE, HEIGHT};
    SDL_RenderFillRect(renderer, &rightBorder);

    // Create a clip rectangle for the visible area
    SDL_Rect clipRect = {
        BORDER_SIZE, 
        BORDER_SIZE, 
        WIDTH - 2 * BORDER_SIZE, 
        HEIGHT - 2 * BORDER_SIZE
    };
    SDL_RenderSetClipRect(renderer, &clipRect);

    // Render tiles
    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            int tileIndex = mapData[y][x] - 1;
            if (tileIndex >= 0 && tileIndex < tiles.size())
            {
                SDL_Rect destRect = {
                    x * displayTileSize - camX + BORDER_SIZE,
                    y * displayTileSize - camY + BORDER_SIZE,
                    displayTileSize,
                    displayTileSize
                };
                SDL_RenderCopy(this->renderer, this->tileAtlasTexture, &this->tiles[tileIndex], &destRect);
            }
        }
    }

    // Reset clip rectangle
    SDL_RenderSetClipRect(renderer, NULL);
}


void World::updateServer(float *oldX, float *oldY)
{
    if (!this->keep_window_open) return;
    Player* player = this->client->getPlayer();
    // Only Call This if the Player has Moved
    if(*oldX != player->getX() || *oldY != player->getY()) {
        this->client->handlePlayerMoved(player->getID(), player->getX(), player->getY());
        *oldX = player->getX();
        *oldY = player->getY();
    }

    if (!this->sendPlayerTexture) {
        std::cout << "Sending Palyer Texture" << std::endl;
        if (this->playerFrontIdleTexture || this->playerFrontRunTexture)
        {
            // Send The Player Texture to the server
            // We will just send the index that is chosen
            std::cout << "Sending Player Texture Inside" << std::endl;
            if (this->client->handleSendingPlayerTexture(spriteIndex))
            {
                this->sendPlayerTexture = true;
            } else {
                std::cout << "Coudlnt Send Player Texture" << std::endl;
            }
        } else {
            std::cout << "Nothing Yet" << std::endl;
        }
    }

    if (!this->sentMap)
    {
        if (this->mapData.size() > 0)
        {
            this->client->handleSendingMapInfo(this->mapData);
            this->sentMap = true;
        }
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
    if (this->keep_window_open == false) return;
    Player* player = this->client->getPlayer();
    Uint32 now = SDL_GetTicks();
    int feetY = player->getY() + player->getHeight();
    int centerX = player->getX() + static_cast<int>(player->getWidth() / 2);

    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - WIDTH / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - HEIGHT / 2);
    // Allow A Hit box to always be drawn even (this will be invisible)

    if (!player->getIsWalking())
    {
        switch(player->getFacing())
        {
            case Facing::FORWARD:
                std::cout << "Showing Forward" << std::endl;
                this->playerSideIdleSprite.setCurrentFrame(0);
                this->playerBackIdleSprite.setCurrentFrame(0);
                this->playerFrontRunSprite.setCurrentFrame(0);
                this->playerBackRunSprite.setCurrentFrame(0);
                this->playerSideRunSprite.setCurrentFrame(0);

                if (now - this->lastFrameTime >= this->frameDelay)
                {
                    this->playerFrontIdleSprite.setCurrentFrame((this->playerFrontIdleSprite.getCurrentFrame() + 1) % this->playerFrontIdleSprite.getFrameCount());
                    this->lastFrameTime = now;
                }
                if (spriteIndex == 2)
                    Sprite::renderSprite(this->playerFrontIdleSprite, this->renderer, playerFrontIdleTexture, 32, 32, player, this->hitbox);
                else
                    Sprite::renderSprite(this->playerFrontIdleSprite, this->renderer, playerFrontIdleTexture, 30, 45, player, this->hitbox);
                break;
            case Facing::BACKWARD:
                std::cout << "Showing Backward" << std::endl;
                this->playerFrontIdleSprite.setCurrentFrame(0);
                this->playerSideIdleSprite.setCurrentFrame(0);
                this->playerFrontRunSprite.setCurrentFrame(0);
                this->playerSideRunSprite.setCurrentFrame(0);
                this->playerBackRunSprite.setCurrentFrame(0);

                if (now - this->lastFrameTime >= this->frameDelay)
                {
                    this->playerBackIdleSprite.setCurrentFrame((this->playerBackIdleSprite.getCurrentFrame() + 1) % this->playerBackIdleSprite.getFrameCount());
                    this->lastFrameTime = now;
                }
                if (spriteIndex == 2)
                    Sprite::renderSprite(this->playerBackIdleSprite, this->renderer, playerBackIdleTexture, 32, 32, player, this->hitbox);
                else
                    Sprite::renderSprite(this->playerBackIdleSprite, this->renderer, playerBackIdleTexture, 30, 45, player, this->hitbox);
                break;
            case Facing::LEFT:
            case Facing::RIGHT:
                // Set All The Other Sprites to 0
                this->playerFrontIdleSprite.setCurrentFrame(0);
                this->playerBackIdleSprite.setCurrentFrame(0);
                this->playerFrontRunSprite.setCurrentFrame(0);
                this->playerBackRunSprite.setCurrentFrame(0);
                this->playerSideRunSprite.setCurrentFrame(0);

                if (now - this->lastFrameTime >= this->frameDelay)
                {
                    this->playerSideIdleSprite.setCurrentFrame((this->playerSideIdleSprite.getCurrentFrame() + 1) % this->playerSideIdleSprite.getFrameCount());
                    this->lastFrameTime = now;
                }
                if (spriteIndex == 2)
                    Sprite::renderSprite(this->playerSideIdleSprite, this->renderer, playerSideIdleTexture, 32, 32, player, this->hitbox);
                else
                    Sprite::renderSprite(this->playerSideIdleSprite, this->renderer, playerSideIdleTexture, 30, 45, player, this->hitbox);
                break;
        }
    }
    else
    {
        switch (player->getFacing())
        {
            case Facing::FORWARD:
                this->playerSideIdleSprite.setCurrentFrame(0);
                this->playerBackIdleSprite.setCurrentFrame(0);
                this->playerFrontIdleSprite.setCurrentFrame(0);
                this->playerSideRunSprite.setCurrentFrame(0);
                this->playerBackRunSprite.setCurrentFrame(0);
                
                if (now - this->lastFrameTime >= this->frameDelay)
                {
                    this->playerFrontRunSprite.setCurrentFrame((this->playerFrontRunSprite.getCurrentFrame() + 1) % this->playerFrontRunSprite.getFrameCount());
                    this->lastFrameTime = now;
                }
                if (spriteIndex == 2)
                    Sprite::renderSprite(this->playerFrontRunSprite, this->renderer, playerFrontRunTexture, 32, 32, player, this->hitbox);
                else
                    Sprite::renderSprite(this->playerFrontRunSprite, this->renderer, playerFrontRunTexture, 30, 45, player, this->hitbox);
            break;
            case Facing::BACKWARD:
                this->playerFrontIdleSprite.setCurrentFrame(0);
                this->playerSideIdleSprite.setCurrentFrame(0);
                this->playerBackIdleSprite.setCurrentFrame(0);
                this->playerSideRunSprite.setCurrentFrame(0);
                this->playerFrontRunSprite.setCurrentFrame(0);

                if (now - this->lastFrameTime >= this->frameDelay)
                {
                    this->playerBackRunSprite.setCurrentFrame((this->playerBackRunSprite.getCurrentFrame() + 1) % this->playerBackRunSprite.getFrameCount());
                    this->lastFrameTime = now;
                }

                if (spriteIndex == 2)
                    Sprite::renderSprite(this->playerBackRunSprite, this->renderer, playerBackRunTexture, 32, 32, player, this->hitbox);
                else
                    Sprite::renderSprite(this->playerBackRunSprite, this->renderer, playerBackRunTexture, 30, 45, player, this->hitbox);
                break;
            case Facing::LEFT:
            case Facing::RIGHT:
                this->playerFrontIdleSprite.setCurrentFrame(0);
                this->playerBackIdleSprite.setCurrentFrame(0);
                this->playerSideIdleSprite.setCurrentFrame(0);
                this->playerFrontRunSprite.setCurrentFrame(0);
                this->playerBackRunSprite.setCurrentFrame(0);

                if (now - this->lastFrameTime >= this->frameDelay)
                {
                    this->playerSideRunSprite.setCurrentFrame((this->playerSideRunSprite.getCurrentFrame() + 1) % this->playerSideRunSprite.getFrameCount());
                    this->lastFrameTime = now;
                }
                if (spriteIndex == 2)
                    Sprite::renderSprite(this->playerSideRunSprite, this->renderer, playerSideRunTexture, 32, 32, player, this->hitbox);
                else
                    Sprite::renderSprite(this->playerSideRunSprite, this->renderer, playerSideRunTexture, 30, 45, player, this->hitbox);

                break;
        }
    }
    if (DEBUG)
        Sprite::renderDebugHitbox(this->renderer, hitbox);
    if(DEBUG)
    {
        UI::renderTextAtPosition(
            this->renderer,
            this->font_texture,
            this->fonts,
            this->bf.returnBlockInfoByPosition(centerX, feetY, this->mapData),
            20,
            HEIGHT - 25,
            FONT_WIDTH,
            FONT_HEIGHT,
            FONT_SCALE,
            false,
            1
        );
    }
}

void World::drawGreen() { SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255); }
void World::drawBlue() { SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255); }
void World::drawRed() { SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255); }
void World::drawYellow() { SDL_SetRenderDrawColor(this->renderer, 255, 255, 0, 255); }

void World::renderOtherPlayers()
{
    if (this->keep_window_open == false) return;
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
        /*Sprite::renderSprite(this->playerIdleSprite, this->renderer, playerIdleTexture, 30, 45, player);*/
        /**
        this is the default value (-1) were just checking for change
        **/
        bool displayed = false;
        if (remotePlayer->getSpriteIndex() != -1)
        {
            SDL_Texture* player_texture = nullptr;
            if (!Texture::loadTexture(PlayerFrontIdlePaths[remotePlayer->getSpriteIndex()].getPath(), &player_texture, this->renderer))
                std::cout << " Failed to load player texture " << std::endl;

            // if We make it in here we need to change the width and the height of the remotePlayerRect
            remotePlayerRect.w = 30;
            remotePlayerRect.h = 45;

            Sprite::renderSprite(PlayerFrontIdlePaths[remotePlayer->getSpriteIndex()], this->renderer, player_texture, remotePlayerRect, remotePlayer);
            displayed = true;
         }
        if (!displayed)
        {
            SDL_RenderFillRect(renderer, &remotePlayerRect);
        }
    }
}

// Transparent Skinny Box Stetching across the screen
void World::renderCommandInput()
{
    if (!this->keep_window_open) return;
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

void World::checkIfPosIsBreakable()
{
    if(!this->keep_window_open) return;
    int breakableState = this->bf.checkBlockState(this->client->getPlayer()->getWorldHitbox(), this->mapData);
    if (breakableState & BlockState::BREAKABLE)
    {
        UI::renderTextAtPosition(
            this->renderer,
            this->font_texture,
            this->fonts,
            "Press [E] to Break",
            20,
            20,
            FONT_WIDTH,
            FONT_HEIGHT,
            FONT_SCALE,
            false,
            1
        );
        this->allowBreak = true;
    }
}

void World::checkIfPosIsEnterable()
{
    if(!this->keep_window_open) return;
    int enterableState = this->bf.checkBlockState(this->client->getPlayer()->getWorldHitbox(), this->mapData);

    if (enterableState & BlockState::ENTERABLE) 
    {
        this->enterHouse = true;
        UI::renderTextAtPosition(
            this->renderer,
            this->font_texture,
            this->fonts,
            "Press [E] to Enter",
            20,
            20,
            FONT_WIDTH,
            FONT_HEIGHT,
            FONT_SCALE,
            false,
            1
        );
    } 
    else 
    {
        this->enterHouse = false;
    }
}
