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
std::vector<Sprite> PlayerFrontIdlePaths;
std::vector<Sprite> PlayerBackIdlePaths;
std::vector<Sprite> PlayerSideIdlePaths;

std::vector<Sprite> PlayerFrontRunningPaths;
std::vector<Sprite> PlayerBackRunningPaths;
std::vector<Sprite> PlayerSideRunningPaths;
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
    std::string CHAR_PATH = "Characters/";

    // Now, initialize the sprite paths using the correct ASSET_PATH
    PlayerFrontIdlePaths = {
        {"Girl", ASSET_PATH +   CHAR_PATH + "girl-1/front_idle.png", 0, 7},
        {"Boy", ASSET_PATH +    CHAR_PATH + "boy-1/front_idle.png", 0, 5},
        {"Boy2", ASSET_PATH +   CHAR_PATH + "boy-2/front_idle.png", 0, 7}
    };
    PlayerBackIdlePaths = {
        {"Girl", ASSET_PATH +   CHAR_PATH + "girl-1/front_idle.png", 0, 7},
        {"Boy", ASSET_PATH +    CHAR_PATH + "boy-1/front_idle.png", 0, 5},
        {"Boy2", ASSET_PATH +   CHAR_PATH + "boy-2/back_idle.png", 0, 6}
    };
    PlayerSideIdlePaths = {
        {"Girl", ASSET_PATH +   CHAR_PATH + "girl-1/front_idle.png", 0, 7},
        {"Boy", ASSET_PATH +    CHAR_PATH + "boy-1/front_idle.png", 0, 5},
        {"Boy2", ASSET_PATH +   CHAR_PATH + "boy-2/back_idle.png", 0, 7}
    };
    PlayerFrontRunningPaths = {
        {"Girl", ASSET_PATH +   CHAR_PATH + "girl-1/front_idle.png", 0, 7},
        {"Boy", ASSET_PATH +    CHAR_PATH + "boy-1/front_idle.png", 0, 5},
        {"Boy2", ASSET_PATH +   CHAR_PATH + "boy-2/front_walk.png", 0, 4}
    };
    PlayerBackRunningPaths = {
        {"Girl", ASSET_PATH +   CHAR_PATH + "girl-1/front_idle.png", 0, 7},
        {"Boy", ASSET_PATH +    CHAR_PATH + "boy-1/front_idle.png", 0, 5},
        {"Boy2", ASSET_PATH +   CHAR_PATH + "boy-2/back_walk.png", 0, 5}
    };
    PlayerSideRunningPaths = {
        {"Girl", ASSET_PATH +   CHAR_PATH + "girl-1/front_idle.png", 0, 7},
        {"Boy", ASSET_PATH +    CHAR_PATH + "boy-1/front_idle.png", 0, 5},
        {"Boy2", ASSET_PATH +   CHAR_PATH + "boy-2/back_idle.png", 0, 7}
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
