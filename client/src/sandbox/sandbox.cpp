#include "sandbox/sandbox.h"

bool SandBox::sdlInitialized = false;
SandBox::SandBox()
{
    if (!sdlInitialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        sdlInitialized = true;
    }
    this->initWindow();
    this->initRenderer();
    this->isRunning = false;
}

SandBox::~SandBox()
{
    this->cleanup();
}

void SandBox::cleanup()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void SandBox::loadSandBox()
{
    this->isRunning = true;
    SDL_Event e;
    while (this->isRunning)
    {
        while(SDL_PollEvent(&e)){ 
            this->handleEvent(e); 
            if (!this->isRunning) break;
        }
        if (!this->isRunning) break;

        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
        SDL_RenderClear(this->renderer);

        // Show Box
        this->renderOpenFile();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }
    this->cleanup();
}

void SandBox::renderOpenFile()
{
    this->openFileButton.x = this->o_file_button_x;
    this->openFileButton.y = this->o_file_button_y;
    this->openFileButton.w = this->o_file_button_width;
    this->openFileButton.h = this->o_file_button_height;

    SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &this->openFileButton);
}

void SandBox::handleEvent(SDL_Event e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;

        if (this->checkButtonClicked(this->openFileButton, mouseX, mouseY))
        {
            std::cout << "Open File Button Clicked" << std::endl;
        }
    }
    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            std::cout << "SDL_WINDOWEVENT_CLOSE event received." << std::endl;
            this->isRunning = false;
        }
    }
}

bool SandBox::checkButtonClicked(SDL_Rect buttonRect, int mouseX, int mouseY)
{
    return mouseX >= buttonRect.x && mouseX <= (buttonRect.x + buttonRect.w) &&
    mouseY >= buttonRect.y && mouseY <= (buttonRect.y + buttonRect.h);
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
    SandBox* sandbox = static_cast<SandBox*>(instance);
    sandbox->loadSandBox();
    delete sandbox;  // Add this line to properly clean up
}
