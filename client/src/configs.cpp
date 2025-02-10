#include "configs.h"
#include "sprite.h"

// Define the asset path (will be set dynamically)
std::string ASSET_PATH;

// Global sprite animation paths (declared, but initialized later)
std::vector<Sprite> PlayerIdlePaths;
std::vector<Sprite> PlayerRunningPaths;

// Function to initialize the asset paths dynamically
void initializePaths() {
    // Get absolute path of the executable
    std::string exePath = std::filesystem::current_path().string();
    
    // Set ASSET_PATH to the absolute directory of assets
    ASSET_PATH = exePath + "/../Assets/";

    // Now, initialize the sprite paths using the correct ASSET_PATH
    PlayerIdlePaths = {
        {"Girl", ASSET_PATH + "char_idle2.png", 0, 7},
        {"Boy", ASSET_PATH + "char_idle1.png", 0, 5}
    };

    PlayerRunningPaths = {
        {"Girl", ASSET_PATH + "char_idle2.png", 0, 7},
        {"Boy", ASSET_PATH + "char_idle1.png", 0, 5}
    };
}
