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

void Game::renderGui()
{
    if (!this->guiValues.toggleGui) return; // Only render if debug UI is enabled

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("Debug Window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    if (ImGui::Button("Toggle Layers"))
    {
        this->guiValues.showLayerInfo = !this->guiValues.showLayerInfo;
    }
    ImGui::SameLine();
    ImGui::Text(this->guiValues.showLayerInfo ? "On" : "Off");

    for (int i = 0; i < this->layerInfo.size(); i++)
    {
        if (ImGui::Button(std::to_string(i).c_str()))
        {
            this->layerInfo[i] = !this->layerInfo[i];
        }
        ImGui::SameLine();
        ImGui::Text(this->layerInfo[i] ? "On" : "Off");
    }

    // checkbox to allow color for different tsx
    ImGui::Checkbox("Color for different tsx", &this->guiValues.colorForDifferentTexture);
    ImGui::Checkbox("Show Grid Over Textture", &this->guiValues.drawGridOverTexture);

    // Allow slider to change the map scale
    ImGui::SliderFloat("Map Scale", &this->mapScale, 1.0f, 10.0f);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), this->renderer);
}

void Game::initGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
    ImGui_ImplSDLRenderer2_Init(this->renderer);
}

void Game::drawMap()
{
    TSDL::drawMap(this->renderer,this->font ,this->fontNumbers, this->map, this->mapScale, this->layerInfo, this->guiValues); 
}

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

    if (this->guiValues.toggleGui)
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

    // Toggle ImGui UI with Command + D
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d && (SDL_GetModState() & KMOD_GUI))
    {
        this->guiValues.toggleGui = !this->guiValues.toggleGui;
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
