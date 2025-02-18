// DebugGUI.cpp
#include "debug_gui.h"
#include <SDL_error.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

DebugGUI::GUIValues DebugGUI::guiValues;

void DebugGUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


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
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    // want default close
    if(ImGui::CollapsingHeader("Debug Logs"))
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
    }

    if (ImGui::CollapsingHeader("Map Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Change Map (New Json)"))
        {
            // Wanna Open File
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
        if (ImGui::Button("Toggle Layers"))
        {
            guiValues.showLayerInfo = !guiValues.showLayerInfo;
        }
        ImGui::SameLine();
        ImGui::Text(guiValues.showLayerInfo ? "On" : "Off");

        for (int i = 0; i < guiValues.layerInfo.size(); i++)
        {
            if (ImGui::Button(std::to_string(i).c_str()))
            {
                guiValues.layerInfo[i] = !guiValues.layerInfo[i];
            }
            ImGui::SameLine();
            ImGui::Text(guiValues.layerInfo[i] ? "On" : "Off");
        }

        ImGui::Checkbox("Color for different textures", &guiValues.colorForDifferentTexture);
        ImGui::Checkbox("Color for different Layers", &guiValues.colorForDifferentLayer);
        ImGui::Checkbox("Show Grid Over Texture", &guiValues.drawGridOverTexture);
        ImGui::SliderFloat("Map Scale", &guiValues.mapScale, 1.0f, 10.0f);
    }


    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}
