#include "engine/ui/ui_manager.h"
#include "engine/ui/file_explorer.h"
#include "engine/ui/inspector.h"
#include "engine/ui/toolbar.h"


#include "imgui.h"

UIManager::UIManager()
{
    elements.push_back(new FileExplorer());
    elements.push_back(new Inspector());
    elements.push_back(new Toolbar());
}

UIManager::~UIManager()
{
    for (UIElement* element : elements)
    {
        delete element;
    }
    elements.clear();
}

void UIManager::RenderUI()
{
    // Setup docking (if needed)
    if (ImGui::GetMainViewport()) {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id);
    }
    // Render each UI element
    for (auto element : elements) {
        element->Render();
    }
}
