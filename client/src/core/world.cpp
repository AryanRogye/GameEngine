#include "world.h"

World::World(
    SDL_Renderer* renderer,
    SDL_Window* window
)
// Setting the player Run Sprites Through Here
: playerRunSprite(PlayerRunningPaths[spriteIndex]), playerIdleSprite(PlayerIdlePaths[spriteIndex])
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
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        handlePlayerMovement(state, dt);
        // Kinda Like Minecraft Black Box Chat thing
        this->renderCommandInput();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }
}


void World::handlePlayerMovement(const Uint8* state, float dt) {
    if (this->commandMode) return;  // Don't move if in command mode

    Player *player = this->client->getPlayer();
    float newX = player->getX();
    float newY = player->getY();
    float speed = player->getSpeed();  // pixels per second

    bool moving = false;

    // Store original position
    float originalX = newX;
    float originalY = newY;

    if (state[SDL_SCANCODE_W]) { newY -= speed * dt; moving = true; }
    if (state[SDL_SCANCODE_A]) { newX -= speed * dt; player->setFacingRight(false); moving = true; }
    if (state[SDL_SCANCODE_S]) { newY += speed * dt; moving = true; }
    if (state[SDL_SCANCODE_D]) { newX += speed * dt; player->setFacingRight(true); moving = true; }

    player->setIsWalking(moving);

    // Update position and hitbox - note we can offset the hitbox however we want
    int hitboxOffsetX = 0;  // Adjust these offsets to position the hitbox
    int hitboxOffsetY = 0;  // wherever you want relative to player position

    // Try horizontal movement
    player->setX(newX);
    player->updateWorldHitbox(hitboxOffsetX, hitboxOffsetY);
    if (this->bf.checkCollision(player->getWorldHitbox(), this->mapData)) {
        player->setX(originalX);
    }

    // Try vertical movement
    player->setY(newY);
    player->updateWorldHitbox(hitboxOffsetX, hitboxOffsetY);
    if (this->bf.checkCollision(player->getWorldHitbox(), this->mapData)) {
        player->setY(originalY);
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
        if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) {
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

    Player* player = this->client->getPlayer();

    // Camera follows player, centered
    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - WIDTH / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - HEIGHT / 2);

    // **Calculate which tiles are visible on the screen**
    int startX = std::max(0, camX / displayTileSize);
    int startY = std::max(0, camY / displayTileSize);
    int endX = std::min(mapWidthInTiles, (camX + WIDTH) / displayTileSize + 1);
    int endY = std::min(mapHeightInTiles, (camY + HEIGHT) / displayTileSize + 1);

    // **Loop only through visible tiles**
    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            int tileIndex = mapData[y][x] - 1;

            if (tileIndex >= 0 && tileIndex < tiles.size())
            {
                SDL_Rect destRect = {
                    x * displayTileSize - camX,  // Apply camera offset
                    y * displayTileSize - camY,
                    displayTileSize,
                    displayTileSize
                };

                SDL_RenderCopy(this->renderer, this->tileAtlasTexture, &this->tiles[tileIndex], &destRect);
            }
        }
    }
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

    if (!this->sendPlayerTexture) {
        std::cout << "Sending Palyer Texture" << std::endl;
        if (this->playerIdleTexture || this->playerRunTexture)
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

    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - WIDTH / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - HEIGHT / 2);
    // Allow A Hit box to always be drawn even (this will be invisible)

    if (!player->getIsWalking())
    {
        // First Thing Reset the Running Frame
        this->playerRunSprite.setCurrentFrame(0);
        if (now - this->lastFrameTime >= this->frameDelay)
        {
            this->playerIdleSprite.setCurrentFrame((this->playerIdleSprite.getCurrentFrame() + 1) % this->playerIdleSprite.getFrameCount());
            this->lastFrameTime = now;
        }
        Sprite::renderSprite(this->playerIdleSprite, this->renderer, playerIdleTexture, 30, 45, player, this->hitbox);
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
        Sprite::renderSprite(this->playerRunSprite, this->renderer, this->playerRunTexture, 30, 45, player, this->hitbox);
        this->bf.printBlockInfoByPosition(centerX, feetY, this->mapData);
    }
    if (DEBUG)
        Sprite::renderDebugHitbox(this->renderer, hitbox);
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
        /*Sprite::renderSprite(this->playerIdleSprite, this->renderer, playerIdleTexture, 30, 45, player);*/
        /**
        this is the default value (-1) were just checking for change
        **/
        bool displayed = false;
        if (remotePlayer->getSpriteIndex() != -1)
        {
            SDL_Texture* player_texture = nullptr;
            if (!Texture::loadTexture(PlayerIdlePaths[remotePlayer->getSpriteIndex()].getPath(), &player_texture, this->renderer))
                std::cout << " Failed to load player texture " << std::endl;

            // if We make it in here we need to change the width and the height of the remotePlayerRect
            remotePlayerRect.w = 30;
            remotePlayerRect.h = 45;

            Sprite::renderSprite(PlayerIdlePaths[remotePlayer->getSpriteIndex()], this->renderer, player_texture, remotePlayerRect, remotePlayer);
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
