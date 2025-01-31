#include "shared/game.h"

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

void Game::start_game()
{
    this->client->handlePlayerJoined(this->client->getPlayer()->getName());
    float oldX = this->client->getPlayer()->getX();
    float oldY = this->client->getPlayer()->getY();

    while(this->keep_window_open)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            this->handleEvent(e);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Constanty send Player Position to Server
        this->updateServer(&oldX,&oldY);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Game::updateServer(float *oldX, float *oldY)
{
    this->renderPlayer();
    Player* player = this->client->getPlayer();
    // Only Call This if the Player has Moved
    if(*oldX != player->getX() || *oldY != player->getY()) {
        this->client->handlePlayerMoved(player->getId(), player->getX(), player->getY());
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
