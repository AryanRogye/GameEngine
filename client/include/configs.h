#pragma once

#define WIDTH 800
#define HEIGHT 600
#define PORT 8080
#define TILE_SIZE 64
#define PLAYER_WIDTH 48
#define PLAYER_HEIGHT 56
#define FONT_WIDTH 8
#define FONT_HEIGHT 7
#define FONT_SCALE 3

#include <string>
#include <filesystem>  // C++17 feature
#include <vector>

// Global asset path
extern std::string ASSET_PATH;

// Global vectors for character animations
class Sprite;
extern std::vector<Sprite> PlayerIdlePaths;
extern std::vector<Sprite> PlayerRunningPaths;
extern int spriteIndex;

// Function to initialize asset paths
void initializePaths();
