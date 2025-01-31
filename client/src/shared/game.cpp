#include "shared/game.h"

Game::Game() {
    this->player = new Player(0, "Player 1");
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
    playerRect.x = this->player->getX();
    playerRect.y = this->player->getY();
    playerRect.w = this->player->getWidth();
    playerRect.h = this->player->getHeight();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerRect);
}

void Game::start_game()
{
    this->client->handlePlayerJoined(this->player->getName());
    while(this->keep_window_open)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            this->handleEvent(e);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Constanty send Player Position to Server
        this->updateServer();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Game::updateServer()
{
    this->renderPlayer();
}


void Game::handleEvent(SDL_Event e)
{
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
            this->player->setY(this->player->getY() - 1);
            break;
        case SDLK_a:
            std::cout << "a pressed" << std::endl;
            // Move player left
            this->player->setX(this->player->getX() - 1);
            break;
        case SDLK_s:
            std::cout << "s pressed" << std::endl;
            // Move player down
            this->player->setY(this->player->getY() + 1);
            break;
        case SDLK_d:
            std::cout << "d pressed" << std::endl;
            // Move player right
            this->player->setX(this->player->getX() + 1);
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
