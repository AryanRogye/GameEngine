#include "engine/GameEngine.h"


#include "engine/ui/ui_manager.h"
#include "debug_gui.h"



#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"




GameEngine::GameEngine()
{
    std::cout << "Initializing GameEngine..." << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cerr << "Error: SDL could not initialize! " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("Comfy Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer)
    {
        std::cerr << "Error: SDL could not create window/renderer! " << SDL_GetError() << std::endl;
        return;
    }

    // can allow the window the be resizable
    SDL_SetWindowResizable(window, SDL_TRUE);
    // set first maximized
    SDL_MaximizeWindow(window);

    // ✅ Use DebugGUI to initialize ImGui instead of doing it manually
    /*DebugGUI::Init(window, renderer);*/


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


    ImGui::LoadIniSettingsFromDisk("imgui.ini");

    game = new Game();
    game->setRenderer(renderer);

    std::cout << "GameEngine initialized successfully!" << std::endl;
}

GameEngine::~GameEngine()
{
    std::cout << "Shutting down GameEngine..." << std::endl;

    ImGui::SaveIniSettingsToDisk("imgui.ini");

    // ✅ Shutdown ImGui via DebugGUI
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameEngine::run()
{
    running_engine = true;
    Uint32 lastTicks = SDL_GetTicks();

    while (running_engine)
    {
        // Calculate delta time
        Uint32 nowTicks = SDL_GetTicks();
        float dt = (nowTicks - lastTicks) * 0.001f;
        lastTicks = nowTicks;

        // Handle events
        handleEvents();
        
        // Update game logic
        if (game)
            game->update(dt);
            
        // Render everything
        render();

        // Cap frame rate
        SDL_Delay(16);
    }
}

void GameEngine::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
        {
            running_engine = false;
        }

        // Pass events to the game
        if (game)
            game->handleEvent(event, 0.016f); // Using fixed dt for simplicity
    }
}

static bool initialized = false;
void GameEngine::render()
{
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // Start a new ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();  // MANDATORY



    // Create a DockSpace for UI layout
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

    if (!initialized)
    {
        CreateDockLayout();
        initialized = true;
    }

    RenderGameViewport();

    // Render UI elements using UIManager
    static UIManager uiManager;
    uiManager.RenderUI();

    // Render ImGui draw data
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

    // Present renderer
    SDL_RenderPresent(renderer);
}

void GameEngine::RenderGameViewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // Remove padding
    ImGui::Begin("Viewport");

    // Get the available size of the ImGui viewport
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

    // Convert to integer size for SDL rendering
    int width = (int)viewportPanelSize.x;
    int height = (int)viewportPanelSize.y;

    // Ensure the SDL render target matches the ImGui viewport size
    if (!gameTexture || width != lastWidth || height != lastHeight)
    {
        if (gameTexture)
            SDL_DestroyTexture(gameTexture);

        gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        lastWidth = width;
        lastHeight = height;
        
        // Update viewport size in game
        if (game) {
            game->viewportWidth = width;
            game->viewportHeight = height;
            game->gameViewport.w = width;
            game->gameViewport.h = height;
        }
    }

    // Set SDL to render into the gameTexture
    SDL_SetRenderTarget(renderer, gameTexture);
    
    // Render the actual game into this texture (just one frame)
    if (game) {
        game->start_game();
    }

    // Reset SDL render target
    SDL_SetRenderTarget(renderer, NULL);

    // Display the SDL texture inside ImGui
    ImGui::Image((ImTextureID)gameTexture, viewportPanelSize, ImVec2(0, 0), ImVec2(1, 1));

    ImGui::End();
    ImGui::PopStyleVar();
}

void GameEngine::CreateDockLayout()
{
    // Get the main docking space ID
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");

    // Remove any existing layout
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

    // Split the dock into 4 areas
    ImGuiID dock_main = dockspace_id;  // Root dock
    ImGuiID dock_top, dock_left, dock_right, dock_center, dock_left_extra, dock_bottom;

    dock_top    = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up,    0.1f, NULL, &dock_main);
    dock_left   = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left,  0.2f, NULL, &dock_main);
    // add a extra left
    dock_left_extra = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, NULL, &dock_main);
    dock_right  = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, NULL, &dock_main);
    dock_center = dock_main; // The remaining space is the center viewport.

    dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.1f, NULL, &dock_main);

    // Dock specific windows into their respective areas
    ImGui::DockBuilderDockWindow("Toolbar", dock_top);
    ImGui::DockBuilderDockWindow("File Explorer", dock_left);
    ImGui::DockBuilderDockWindow("Console", dock_left_extra);
    ImGui::DockBuilderDockWindow("Inspector", dock_right);
    ImGui::DockBuilderDockWindow("Viewport", dock_center);
    ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_left_extra);

    // Finalize layout
    ImGui::DockBuilderFinish(dockspace_id);
}
