#include "sandbox/sandbox.h"
#include <SDL_events.h>

SandBox::SandBox()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to Initialize SDL2 Library" << std::endl;
        return;
    }
    this->initWindow();
    this->initRenderer();
    this->isRunning = false;
}

void SandBox::loadSandBox()
{
    this->isRunning = true;
    SDL_Event e;
    while (this->isRunning)
    {
        while(SDL_PollEvent(&e)){ this->handleEvent(e); }

        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
        SDL_RenderClear(this->renderer);

        // Show Box
        SDL_Rect rect = { 100, 100, 50, 50 };
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(this->renderer, &rect);


        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void SandBox::handleEvent(SDL_Event e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        // CHeck if the box is clicked
        if (e.button.x >= 100 && e.button.x <= 150 &&
            e.button.y >= 100 && e.button.y <= 150)
        {
            std::cout << "Box Clicked" << std::endl;
        }
    }
    if (e.type == SDL_QUIT) {
        std::cout << "SDL_QUIT event received." << std::endl;
        this->isRunning = false;
    } 
    else if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            std::cout << "SDL_WINDOWEVENT_CLOSE event received." << std::endl;
            this->isRunning = false;
        }
    }
}

void SandBox::initWindow()
{
    this->window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    /*SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);*/

    if (!this->window)
    {
        printf("Failed To Make SandBox Window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
}

void SandBox::initRenderer()
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

extern "C" SandBox* createSandBox()
{
    return new SandBox();
}

extern "C" void runSandBox(void* instance)
{
    static_cast<SandBox*>(instance)->loadSandBox();
}
