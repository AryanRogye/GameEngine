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
extern std::string DATA_PATH;

// Global vectors for character animations
class Sprite;
extern std::vector<Sprite> PlayerFrontIdlePaths;
extern std::vector<Sprite> PlayerBackIdlePaths;
extern std::vector<Sprite> PlayerSideIdlePaths;
extern std::vector<Sprite> PlayerFrontRunningPaths;
extern std::vector<Sprite> PlayerBackRunningPaths;
extern std::vector<Sprite> PlayerSideRunningPaths;

extern bool DEBUG;
extern int spriteIndex;

extern int hitboxWidth;
extern int hitboxHeight;
extern int hitboxOffsetX;
extern int hitboxOffsetY;

extern void setHitBoxWidth(int width);
extern void setHitBoxHeight(int height);
extern void setOffsetX(int x);
extern void setOffsetY(int y);
extern void loadHitboxConfig();
extern void saveConfig();

// Function to initialize asset paths
void initializePaths();
