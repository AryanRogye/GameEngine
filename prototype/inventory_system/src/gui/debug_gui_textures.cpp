#include "debug_gui.h"

void DebugGUI::renderTexturesInfo()
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
}
