#include "configs.h"
#include "sprite.h"

// Define the asset path (will be set dynamically)
std::string ASSET_PATH;

// Global sprite animation paths (declared, but initialized later)
std::vector<Sprite> PlayerIdlePaths;
std::vector<Sprite> PlayerRunningPaths;
bool DEBUG;
int spriteIndex;


// Correct definitions (Ensure these are not `extern` here)
int hitboxWidth = 50;
int hitboxHeight = 50;
int hitboxOffsetX = 0;
int hitboxOffsetY = 0;

void setOffsetX(int x)
{
    hitboxOffsetX = x;
}
void setOffsetY(int y)
{
    hitboxOffsetY = y;
}

void setHitBoxWidth(int width) {
    hitboxWidth = width;
}

void setHitBoxHeight(int height) {
    hitboxHeight = height;
}

void initializePaths() {
    hitboxWidth = 50;
    hitboxHeight = 50;
    hitboxOffsetX = 0;
    hitboxOffsetY = 0;

    std::string exePath = std::filesystem::current_path().string();
    spriteIndex = 0;
    DEBUG = false;
    
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
