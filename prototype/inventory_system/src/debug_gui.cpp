// DebugGUI.cpp
#include "debug_gui.h"
#include "imgui.h"

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


void DebugGUI::addDebugLog(const std::string& log, bool copyClipboard ,const std::string& label)
{
    std::string toAdd = "";
    if (label != "")
    {
        toAdd += "(" + label + ") ";
    }
    toAdd += log;

    const std::string timestamp = getTimeStamp();
    guiValues.debugLogs.emplace_back(timestamp + " | " + toAdd, copyClipboard);
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
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    // =====================================================================================================================
    // Mouse Position
    // =====================================================================================================================
    ImGui::Text("(%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);

    // =====================================================================================================================
    // Map Scale Slider
    // =====================================================================================================================
    ImGui::SliderFloat("Map Scale", &guiValues.mapScale, 1.0f, 10.0f);

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
            for (size_t i = 0; i < guiValues.debugLogs.size(); ++i)
            {
                const auto &[logText, copy] = guiValues.debugLogs[i];
                // If the log should be copied, show the button
                ImGui::TextWrapped("%s", logText.c_str());
                if (copy)
                {
                    std::string buttonText = "Copy##" + std::to_string(i);
                    if (ImGui::Button(buttonText.c_str()))
                    {
                        // Use SDL clipboard for other platforms
                        if (SDL_SetClipboardText(logText.c_str()) == 0)
                        {
                            std::cout << "✅ Successfully copied!" << std::endl;
                        }
                        else
                    {
                            std::cerr << "❌ SDL Clipboard Error: " << SDL_GetError() << std::endl;
                        }
                        addDebugLog("Copied to clipboard", false);
                    }
                }         
            }
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Map Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Map"))
        {
            // =====================================================================================================================
            // Display Map Name
            // =====================================================================================================================
            // trim just till the last slash
            size_t lastSlash = guiValues.mapName.find_last_of('/');
            std::string name =  guiValues.mapName.substr(lastSlash + 1);
            ImGui::Text("Map Name: %s", name.c_str());

            // =====================================================================================================================
            // Change Map
            // =====================================================================================================================
            if (ImGui::Button("Change Map (New Json)"))
            {
                std::string path;
                #if defined(__APPLE__)
                    char buffer[1024]; // Buffer to store path
                    FILE* pipe = popen("osascript -e 'POSIX path of (choose file with prompt \"Select a Project Folder:\")'", "r");
                    if (pipe) {
                        if(fgets(buffer, sizeof(buffer), pipe))
                        {
                            buffer[strcspn(buffer, "\n")] = 0;
                            path = buffer;
                        }
                        pclose(pipe);
                    }
                #endif
                    if (!path.empty()) {
                        loadMapConfigs(path);
                    }
            }
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Layers Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Layers"))
        {
            // =====================================================================================================================
            // Toggle Layers
            // =====================================================================================================================
            if (ImGui::Button("Toggle Layers"))
            {
                guiValues.showLayerInfo = !guiValues.showLayerInfo;
            }
            ImGui::SameLine();
            ImGui::Text(guiValues.showLayerInfo ? "On" : "Off");
            ImGui::Checkbox("Color for different Layers", &guiValues.colorForDifferentLayer);

            // =====================================================================================================================
            // Layer Visibility
            // =====================================================================================================================
            if (ImGui::TreeNode("Layer Visibility"))
            {
                for (size_t i = 0; i < guiValues.layerInfo.size(); i++)
                {
                    std::string buttonLabel = (guiValues.layerInfo[i] ? "🟢 " : "🔴 ") + std::to_string(i);
                    if (ImGui::Button(buttonLabel.c_str()))
                    {
                        guiValues.layerInfo[i] = !guiValues.layerInfo[i];
                    }

                    if (i % 4 != 3) ImGui::SameLine(); // Arrange in rows of 4
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            // =====================================================================================================================
            // Layer Settings
            // =====================================================================================================================
            ImGui::Text(
                guiValues.currentMouseLayer == -1 
                ? "Not Set/Implimented Yet"
                : "Current Mouse Layer: %d",
                guiValues.currentMouseLayer
            );
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Textures Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Textures"))
        {
            // =====================================================================================================================
            // Image of Tile
            // =====================================================================================================================
            if (guiValues.currentTileTexture)
            {
                ImGui::Image((ImTextureID)guiValues.currentTileTexture, ImVec2(100, 100));
            }
            // =====================================================================================================================
            // Texture Name
            // =====================================================================================================================
            ImGui::Text(
                guiValues.currentTextureName == "" 
                ? "Not Set/Implimented Yet"
                : "Texture: %s",
                guiValues.currentTextureName.c_str()
            );
            // =====================================================================================================================
            // Texture Settings
            // =====================================================================================================================
            ImGui::Checkbox("Color for different textures", &guiValues.colorForDifferentTexture);
            if (guiValues.colorForDifferentTexture)
            {
                guiValues.showLayerInfo = true;
            }
            // =====================================================================================================================
            // Mouse Tile
            // =====================================================================================================================
            ImGui::Text(
                guiValues.currentMouseTileX == -1 || guiValues.currentMouseTileY == -1 
                ? "Not Set/Implimented Yet"
                : "Current Mouse Tile: (%d, %d)",
                guiValues.currentMouseTileX,
                guiValues.currentMouseTileY
            );
            ImGui::Checkbox("Show Grid Over Texture", &guiValues.drawGridOverTexture);

            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // 
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Player"))
        {
            
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void DebugGUI::SetMapName(const std::string& mapName) { guiValues.mapName = mapName; }
