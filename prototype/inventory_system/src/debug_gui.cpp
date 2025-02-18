// DebugGUI.cpp
#include "debug_gui.h"

DebugGUI::GUIValues DebugGUI::guiValues;
std::vector<bool> DebugGUI::layerInfo;

void DebugGUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
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

    if (ImGui::CollapsingHeader("Map Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Toggle Layers"))
        {
            guiValues.showLayerInfo = !guiValues.showLayerInfo;
        }
        ImGui::SameLine();
        ImGui::Text(guiValues.showLayerInfo ? "On" : "Off");

        for (int i = 0; i < layerInfo.size(); i++)
        {
            if (ImGui::Button(std::to_string(i).c_str()))
            {
                layerInfo[i] = !layerInfo[i];
            }
            ImGui::SameLine();
            ImGui::Text(layerInfo[i] ? "On" : "Off");
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
