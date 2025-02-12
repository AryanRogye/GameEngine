#include "configs.h"

// Keep Local Includes Other Files use the same path
#include "sprite.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

// Define the asset path (will be set dynamically)
std::string ASSET_PATH;
std::string DATA_PATH;

// Global sprite animation paths (declared, but initialized later)
std::vector<Sprite> PlayerIdlePaths;
std::vector<Sprite> PlayerRunningPaths;
bool DEBUG;
int spriteIndex;


// Correct definitions (Ensure these are not `extern` here)
int hitboxWidth =   0;
int hitboxHeight =  0;
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

    std::string exePath = std::filesystem::current_path().string();
    spriteIndex = 0;
    DEBUG = false;
    
    // Set ASSET_PATH to the absolute directory of assets
    DATA_PATH = exePath + "/../Data/configs.ini";
    loadHitboxConfig();
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

void loadHitboxConfig() {
    std::ifstream configFile(DATA_PATH);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open " << DATA_PATH << ". Using defaults.\n";
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                try {
                    if (key == "hitbox_width") hitboxWidth = std::stoi(value);
                    else if (key == "hitbox_height") hitboxHeight = std::stoi(value);
                    else if (key == "hitbox_offset_x") hitboxOffsetX = std::stoi(value);
                    else if (key == "hitbox_offset_y") hitboxOffsetY = std::stoi(value);
                } catch (const std::exception& e) {
                    std::cerr << "Invalid value for " << key << ": " << value << "\n";
                }
            }
        }
    }
}

void saveConfig() {
    std::ofstream configFile(DATA_PATH);
    if (!configFile.is_open()) {
        std::cerr << "Failed to write " << DATA_PATH << "\n";
        return;
    }
    configFile << "hitbox_width=" << hitboxWidth << "\n";
    configFile << "hitbox_height=" << hitboxHeight << "\n";
    configFile << "hitbox_offset_x=" << hitboxOffsetX << "\n";
    configFile << "hitbox_offset_y=" << hitboxOffsetY << "\n";

    configFile.close();
}
