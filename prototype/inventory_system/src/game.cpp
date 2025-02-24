#include "game.h"
#include "comfy_lib.h"
#define WIDTH 800
#define HEIGHT 600

void Game::start_game()
{

    std::cout << "======================================" << std::endl;
    std::cout << "Game Started" << std::endl;
    std::cout << "======================================" << std::endl;

    this->running = true;
    time_t lastTime = -1;
    time_t currentTime = 0;

    // This is used to get delta time
    Uint32 lastTicks = SDL_GetTicks();

    while (this->running)
    {
        Uint32 nowTicks = SDL_GetTicks();
        float dt = (nowTicks - lastTicks) * 0.001f;

        SDL_Event e; while(SDL_PollEvent(&e)) this->handleEvent(e, dt);  
        this->player->update(dt);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);
        SDL_RenderClear(renderer);

        // Hot Reload
        fileChanged(currentTime);
        if (lastTime != currentTime)
        {
            DebugGUI::addDebugLog("File Changed", ErrorCode::SUCCESS);
            this->loadMap();
            this->player->setTileMap(this->map);
            lastTime = currentTime;
        }
        // Draw Here
        this->drawMap();
        this->player->draw(dt, this->gameScale);
        this->renderGui();

        SDL_RenderPresent(this->renderer);
        lastTicks = nowTicks;
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

// want mouse as well

void Game::drawMap() { 
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (this->map != nullptr)
    {
      TSDL::drawMap(this->renderer, this->font, this->fontNumbers, this->map,
                    mouseX, mouseY, this->gameScale, this->player->getCamera());
    }
}
void Game::loadMap()
{
    
    // TODO: Gonna change logic to now load through Data/map_data.ini file which will have the path
    std::string path;
    fetchMapConfigs(path);

    if (path.empty())
    {
        DebugGUI::addDebugLog("No Path Found", ErrorCode::MAP_ERROR);
        // Still Resize To 0
        this->map = new TSDL_TileMap();
        // And Resize the layerInfo to 0
        DebugGUI::guiValues.layerInfo.resize(TSDL::getLayersSize(*this->map));
        std::fill(DebugGUI::guiValues.layerInfo.begin(),
                  DebugGUI::guiValues.layerInfo.end(), true);
        std::cout << "❌ No Path Found" << std::endl;
        return;
    }

    // Load Debug Gui Values
    // turn vector of bool to all false

    // clear the map
    this->map = new TSDL_TileMap();

    if (!TSDL::loadMap(this->renderer, this->map, path, path))
    {
        std::cout << "❌ Failed to load map" << std::endl;
        return;
    }
    DebugGUI::SetMapName(path);
    DebugGUI::guiValues.layerInfo.resize(TSDL::getLayersSize(*this->map));
    std::fill(DebugGUI::guiValues.layerInfo.begin(), DebugGUI::guiValues.layerInfo.end(), true);
}

void Game::handleEvent(SDL_Event e, float dt)
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
    if (this->player)
        this->player->handleInput(e, dt);
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

    this->font = TTF_OpenFont("/Users/aryanrogye/Library/Fonts/FiraCodeNerdFontMono-Regular.ttf", 10);
    if (!font)
    {
        std::cerr << "❌ Error: Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    this->initWindow();
    this->initRenderer();
    this->loadFontNumbers();
    this->initGui();

    this->gameScale = 2.0f;
    this->player = new Player();
    player->setRenderer(this->renderer);

    DebugGUI::setMapScale(&this->gameScale);

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
            DebugGUI::addDebugLog("Failed to create surface from text", ErrorCode::SURFACE_ERROR);
            return;
        }
        this->fontNumbers[i] = SDL_CreateTextureFromSurface(this->renderer, surface);
        if (!this->fontNumbers[i])
        {
            DebugGUI::addDebugLog("Failed to create texture from surface", ErrorCode::TEXTURE_ERROR);
            return;
        }
        SDL_FreeSurface(surface);
    }
    DebugGUI::addDebugLog("Loaded Font Numbers", ErrorCode::NONE);
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
    // maximize window
    SDL_MaximizeWindow(this->window);
    DebugGUI::addDebugLog("Window Made Resizable" , ErrorCode::NONE);
}

void Game::initRenderer()
{
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!this->renderer)
    {
        printf("Failed To Get the surface from the window");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    int vsync = SDL_GetHintBoolean(SDL_HINT_RENDER_VSYNC, SDL_FALSE);
    if (vsync)
    {
        DebugGUI::addDebugLog("Vsync is enabled", ErrorCode::NONE);
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    }
    else
    {
        DebugGUI::addDebugLog("Vsync is disabled", ErrorCode::NONE);
    }
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    DebugGUI::addDebugLog("Monitor Resolution: " + std::to_string(mode.w) + "x" + std::to_string(mode.h), ErrorCode::NONE);
}

void Game::initGui() { DebugGUI::Init(this->window, this->renderer); }
void Game::renderGui() { DebugGUI::Render(this->renderer); }
