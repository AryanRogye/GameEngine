#include "engine/ui/file_explorer.h"

#include "imgui.h"

void FileExplorer::Render()
{
    // get the current viewport size
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowDockID(1, ImGuiCond_FirstUseEver);  // Attach to left dock
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("File Explorer", NULL, window_flags);
    ImGui::Text("File Explorer Content Here");
    ImGui::End();
}
