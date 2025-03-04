#include "debug_gui.h"

void DebugGUI::renderLayerInfo()
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
}
