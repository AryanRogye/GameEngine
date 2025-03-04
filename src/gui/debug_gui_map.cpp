#include "debug_gui.h"

void DebugGUI::renderMapInfo()
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
}
