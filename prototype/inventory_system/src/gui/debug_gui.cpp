// DebugGUI.cpp
#include "debug_gui.h"
#include "imgui.h"
#include "utils/collision.h"
#include "TSDL.h"
#include "utils/sprite.h"
#include <SDL_render.h>
#include <SDL_surface.h>
#include <cstddef>
#include <type_traits>

DebugGUI::GUIValues DebugGUI::guiValues;

void DebugGUI::showSelectedSDLTexture(SDL_Texture* texture, int x, int y, int w, int h)
{
    if (!texture) return;

    SDL_Renderer* renderer = SDL_GetRenderer(SDL_GetWindowFromID(1));
    if (!renderer) return;

    // Cleanup previous texture
    if (guiValues.currentTileTexture) 
    {
        SDL_DestroyTexture(guiValues.currentTileTexture);
        guiValues.currentTileTexture = nullptr;
    }

    // Create a sub-texture to store the portion
    SDL_Texture* subTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    SDL_SetRenderTarget(renderer, subTexture);

    SDL_Rect srcRect = {x, y, w, h};
    SDL_Rect destRect = {0, 0, w, h};
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    SDL_SetRenderTarget(renderer, nullptr);

    // Store the new texture for ImGui
    guiValues.currentTileTexture = subTexture;
}
void DebugGUI::ApplyGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f; // Rounded corners
    style.WindowRounding = 10.0f;
    style.FramePadding = ImVec2(5, 5);
    style.ItemSpacing = ImVec2(8, 8);

    ImGui::StyleColorsDark(); // Base theme
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.6f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.7f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.5f, 0.9f, 1.0f);
}
void DebugGUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    ApplyGuiStyle();

    io.Fonts->AddFontFromFileTTF("/Library/Fonts/SF-Pro.ttf", 16.0f);
    ImGui_ImplSDLRenderer2_CreateFontsTexture();
}


void DebugGUI::Render(SDL_Renderer* renderer)
{
    if (!guiValues.toggleGui) return;

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("Debug Window", nullptr);
    // =====================================================================================================================
    // Header
    // =====================================================================================================================
    ImGui::BeginChild("Header", ImVec2(0, 150), true);
    // =====================================================================================================================
    // FPS Display
    // =====================================================================================================================
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5)); // Adjust spacing
    ImGui::Columns(2);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::NextColumn();

    // =====================================================================================================================
    // Monitor Display
    // =====================================================================================================================
    ImGui::Text("Monitor Resolution: %dx%d", guiValues.monitorWidth, guiValues.monitorHeight);
    ImGui::Columns(1);
    ImGui::PopStyleVar(); // Reset spacing

    // =====================================================================================================================
    // Mouse Position
    // =====================================================================================================================
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5)); // Adjust spacing
    ImGui::Columns(2);
    ImGui::Text("(%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
    ImGui::NextColumn();

    // =====================================================================================================================
    // Vsync Information
    // =====================================================================================================================
    ImGui::Text("Vsync: %s", guiValues.vsync ? "Enabled" : "Disabled");
    ImGui::Columns(1);
    ImGui::PopStyleVar(); // Reset spacing

    // =====================================================================================================================
    // Renderer Name
    // =====================================================================================================================
    ImGui::Text("Renderer: %s", guiValues.rendererName.c_str());

    // =====================================================================================================================
    // Map Scale Slider
    // =====================================================================================================================
    ImGui::SliderFloat("Map Scale", guiValues.mapScale, 1.0f, 10.0f);

    // =====================================================================================================================
    // Recompile & Restart Button
    // =====================================================================================================================
    if (ImGui::Button("Recompile & Restart"))
    {
        std::cout << "🔄 Recompiling & Restarting..." << std::endl;
        system("./../compile.sh &"); // Adjust this command as needed
        exit(0);
    }
    ImGui::EndChild();

    // =====================================================================================================================
    // Debug Tabs
    // =====================================================================================================================
    if (ImGui::BeginTabBar("Debug Tabs"))
    {
        // =====================================================================================================================
        // Log Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Logs"))
        {
            renderLogs();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Map Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Map"))
        {
            renderMapInfo();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Layers Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Layers"))
        {
            renderLayerInfo();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Textures Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Textures"))
        {
            renderTexturesInfo();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Player Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Player"))
        {
            renderPlayerInfo(renderer);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}


void DebugGUI::SetMapName(const std::string& mapName) { guiValues.mapName = mapName; }
void DebugGUI::SetPlayer(Player* player) { guiValues.player = player; }
void DebugGUI::setMapScale(float *scale) { guiValues.mapScale = scale; }
