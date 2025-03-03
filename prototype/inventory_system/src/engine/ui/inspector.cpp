
#include "engine/ui/inspector.h"

#include "imgui.h"

void Inspector::Render()
{
    ImGui::SetNextWindowDockID(2, ImGuiCond_FirstUseEver);  // Attach to right dock
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Inspector", NULL, window_flags);
    ImGui::Text("Inspector Content Here");
    ImGui::End();
}
