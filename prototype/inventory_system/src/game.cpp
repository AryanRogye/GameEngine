#include "game.h"
#include "imgui.h"
#include <SDL_ttf.h>
#define WIDTH 800
#define HEIGHT 600

void Game::start_game()
{

    std::cout << "======================================" << std::endl;
    std::cout << "Game Started" << std::endl;
    std::cout << "======================================" << std::endl;

    this->running = true;
    this->loadMap();
    this->layerInfo.resize(TSDL::getLayersSize(this->map));
    // fill it with false
    // TODO Move out of here to gui
    std::fill(this->layerInfo.begin(), this->layerInfo.end(), true);

    while (this->running)
    {
        SDL_Event e; while(SDL_PollEvent(&e)) this->handleEvent(e);  
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);

        // Draw Here
        this->drawMap();
        this->renderGui();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);

    }

    // Cleanup
    for (int i = 0; i < 300; i++)
    {
        SDL_DestroyTexture(this->fontNumbers[i]);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplSDLRenderer2_Shutdown();

    ImGui::DestroyContext();

    SDL_Quit();
}



void Game::drawMap() { TSDL::drawMap(this->renderer,this->font ,this->fontNumbers, this->map, this->layerInfo);  }
void Game::loadMap()
{
    std::string path = __FILE__;
    path = path.substr(0, path.find_last_of("/"));
    if (!TSDL::loadMap(this->renderer, this->map, path + "/../assets/map.json",  path + "/../assets/"))
    {
        std::cout << "❌ Failed to load map" << std::endl;
        return;
    }
}

void Game::handleEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT)
    {
        this->running = false;
    }

    if (DebugGUI::guiValues.toggleGui)
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

    // Toggle ImGui UI with Command + D
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d && (SDL_GetModState() & KMOD_GUI))
    {
        DebugGUI::guiValues.toggleGui = !DebugGUI::guiValues.toggleGui;
    }
}

Game::Game()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to Initialize SDL2 Library" << std::endl;
        return;
    }

    if (TTF_Init() < 0)
    {
        std::cout << "Failed to Initialize SDL2 TTF Library" << std::endl;
        return;
    }

    this->font = TTF_OpenFont("/Library/Fonts/SF-Pro.ttf", 10);
    if (!font)
    {
        std::cerr << "❌ Error: Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    this->initWindow();
    this->initRenderer();
    this->loadFontNumbers();
    this->initGui();
    this->running = false;
}

void Game::loadFontNumbers()
{
    this->fontNumbers.resize(1000);
    for (int i = 0; i < 1000; i++)
    {
        SDL_Surface *surface = TTF_RenderText_Solid(this->font, std::to_string(i).c_str(), {255, 255, 255});
        if (!surface)
        {
            std::cerr << "❌ Error: Failed to load font numbers: " << TTF_GetError() << std::endl;
            return;
        }
        this->fontNumbers[i] = SDL_CreateTextureFromSurface(this->renderer, surface);
        if (!this->fontNumbers[i])
        {
            std::cerr << "❌ Error: Failed to load font numbers: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_FreeSurface(surface);
    }
    std::cout << "✅ Loaded font numbers" << std::endl;
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

    // make resizeable window
    SDL_SetWindowResizable(this->window, SDL_TRUE);
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

void Game::initGui() { DebugGUI::Init(this->window, this->renderer); }
void Game::renderGui() { DebugGUI::Render(this->renderer); }
