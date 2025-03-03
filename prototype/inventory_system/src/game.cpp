#include "game.h"
#include "comfy_lib.h"
#define WIDTH 800
#define HEIGHT 600

// Add a new update method to handle logic updates
void Game::update(float dt)
{
    // Update game logic
    if (this->player)
        this->player->update(dt);
}

void Game::start_game()
{
    // Initialize if not already done
    static bool initialized = false;
    if (!initialized) {
        std::cout << "Game initialized" << std::endl;
        this->loadMap();
        this->player->setTileMap(this->map);
        initialized = true;
    }

    // Get current time for hot reload
    time_t currentTime = 0;
    static time_t lastTime = -1;

    // Hot Reload check
    fileChanged(currentTime);
    if (lastTime != currentTime)
    {
        DebugGUI::addDebugLog("File Changed", ErrorCode::SUCCESS);
        this->loadMap();
        this->player->setTileMap(this->map);
        lastTime = currentTime;
    }

    // Draw one frame
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the game content
    this->player->getCamera()->update(this->viewportWidth, this->viewportHeight, this->gameScale);
    this->drawMap();
    this->player->draw(0.016f, this->gameScale); // Using fixed dt for now

    std::cout << "Player Position: " << this->player->getPosition().x << " " << this->player->getPosition().y << std::endl;
    std::cout << "Camera Position: " << this->player->getCamera()->getX() << " " << this->player->getCamera()->getY() << std::endl;
    std::cout << "Viewport Width: " << this->viewportWidth << " Viewport Height: " << this->viewportHeight << std::endl;
    std::cout << "Game Scale: " << this->gameScale << std::endl;
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
    /*if (SDL_Init(SDL_INIT_VIDEO) < 0)*/
    /*{*/
    /*    std::cout << "Failed to Initialize SDL2 Library" << std::endl;*/
    /*    return;*/
    /*}*/

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

    /*this->initWindow();*/
    /*this->initRenderer();*/
    /*SDL_RenderSetViewport(this->renderer, &gameViewport);*/

    this->gameViewport = {0, 0, WIDTH, HEIGHT};
    this->viewportWidth = WIDTH;
    this->viewportHeight = HEIGHT;

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
    /*SDL_SetWindowResizable(this->window, SDL_TRUE);*/
    /*// maximize window*/
    /*SDL_MaximizeWindow(this->window);*/
    /*DebugGUI::addDebugLog("Window Made Resizable" , ErrorCode::NONE);*/
}

void Game::initRenderer()
{
    /*this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);*/
    /*// Check valid renderer*/
    /*if(!this->renderer)*/
    /*{*/
    /*    printf("Failed To Get the surface from the window");*/
    /*    SDL_DestroyWindow(window);*/
    /*    SDL_Quit();*/
    /*    return;*/
    /*}*/
    // defining a viewport for the renderer
    gameViewport.x = 0;
    gameViewport.y = 0;
    gameViewport.w = this->viewportWidth;
    gameViewport.h = this->viewportHeight;
    
    /** 
        Debug GUI Renderer Information
    **/
    /*SDL_RendererInfo info;*/
    /*SDL_GetRendererInfo(this->renderer, &info);*/
    /*DebugGUI::guiValues.rendererName = info.name;*/
    /**/
    /*int vsync = SDL_GetHintBoolean(SDL_HINT_RENDER_VSYNC, SDL_FALSE);*/
    /*DebugGUI::guiValues.vsync = vsync;*/
    /*SDL_DisplayMode mode;*/
    /*SDL_GetCurrentDisplayMode(0, &mode);*/
    /**/
    /*DebugGUI::guiValues.monitorWidth = mode.w;*/
    /*DebugGUI::guiValues.monitorHeight = mode.h;*/
}

void Game::setRenderer(SDL_Renderer *engineRenderer)
{
    this->renderer = engineRenderer;
    
    if (this->player)
        this->player->setRenderer(this->renderer);
        
    // Get renderer info for debug
    SDL_RendererInfo info;
    SDL_GetRendererInfo(this->renderer, &info);
    DebugGUI::guiValues.rendererName = info.name;

    int vsync = SDL_GetHintBoolean(SDL_HINT_RENDER_VSYNC, SDL_FALSE);
    DebugGUI::guiValues.vsync = vsync;
}

void Game::initGui() { DebugGUI::Init(this->window, this->renderer); }
void Game::renderGui() { return; DebugGUI::Render(this->renderer); }
