#pragma once

#include <cstdarg>
#include <cstdio>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/_types/_filesec_t.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <chrono>


#include "utils/collision.h"


// #########################################################
// Defines
// #########################################################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

#define BIT(x) 1 << x
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

// #########################################################
// Logging
// #########################################################

enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

enum class LogLevel
{
    TRACE,
    WARN,
    ERROR
};

enum class ErrorCode
{
    // General Errors
    SUCCESS,
    NONE,
    ERROR,
    // File Errors
    FILE_ERROR,
    // Sprite Errors
    SPRITE_ERROR,
    // Texture Errors
    TEXTURE_ERROR,
    // Surface Errors
    SURFACE_ERROR,
    // Map Errors
    MAP_ERROR,
    // Collision Errors
    COLLISION_ERROR,
    // Player Errors
    PLAYER_ERROR,

    JSON_ERROR,
};
// Define an array of all enum values
constexpr std::array<ErrorCode, 11> allErrorCodes = {
    ErrorCode::SUCCESS,
    ErrorCode::NONE,
    ErrorCode::ERROR,
    ErrorCode::FILE_ERROR,
    ErrorCode::SPRITE_ERROR,
    ErrorCode::TEXTURE_ERROR,
    ErrorCode::SURFACE_ERROR,
    ErrorCode::MAP_ERROR,
    ErrorCode::COLLISION_ERROR,
    ErrorCode::PLAYER_ERROR,
    ErrorCode::JSON_ERROR
};

// Function to convert enum to string
inline std::string errorCodeToString(ErrorCode code)
{
    switch (code)
    {
        case ErrorCode::SUCCESS: return "SUCCESS";
        case ErrorCode::NONE: return "NONE";
        case ErrorCode::ERROR: return "ERROR";
        case ErrorCode::FILE_ERROR: return "FILE_ERROR";
        case ErrorCode::SPRITE_ERROR: return "SPRITE_ERROR";
        case ErrorCode::TEXTURE_ERROR: return "TEXTURE_ERROR";
        case ErrorCode::SURFACE_ERROR: return "SURFACE_ERROR";
        case ErrorCode::MAP_ERROR: return "MAP_ERROR";
        case ErrorCode::COLLISION_ERROR: return "COLLISION_ERROR";
        case ErrorCode::PLAYER_ERROR: return "PLAYER_ERROR";
        case ErrorCode::JSON_ERROR: return "JSON_ERROR";
        default: return "UNKNOWN";
    }
}

template <typename... Args>
void _log(const std::string& prefix,const std::string& msg,TextColor textColor,Args... args)
{
    static const std::string TextColorTable[TEXT_COLOR_COUNT] =
    {
        "\033[0;30m", // TEXT_COLOR_BLACK
        "\033[0;31m", // TEXT_COLOR_RED
        "\033[0;32m", // TEXT_COLOR_GREEN
        "\033[0;33m", // TEXT_COLOR_YELLOW
        "\033[0;34m", // TEXT_COLOR_BLUE
        "\033[0;35m", // TEXT_COLOR_MAGENTA
        "\033[0;36m", // TEXT_COLOR_CYAN
        "\033[0;37m", // TEXT_COLOR_WHITE
        "\033[1;30m", // TEXT_COLOR_BRIGHT_BLACK
        "\033[1;31m", // TEXT_COLOR_BRIGHT_RED
        "\033[1;32m", // TEXT_COLOR_BRIGHT_GREEN
        "\033[1;33m", // TEXT_COLOR_BRIGHT_YELLOW
        "\033[1;34m", // TEXT_COLOR_BRIGHT_BLUE
        "\033[1;35m", // TEXT_COLOR_BRIGHT_MAGENTA
        "\033[1;36m", // TEXT_COLOR_BRIGHT_CYAN
        "\033[1;37m"  // TEXT_COLOR_BRIGHT_WHITE
    };
    // Prepare Format String
    std::ostringstream formatBuffer;
    formatBuffer << TextColorTable[textColor] << " " << prefix + " " << msg << " \033[0m\n";
    // Format the String to use variadic arguments
    char textBuffer[8192];
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-security"
    snprintf(textBuffer, sizeof(textBuffer), formatBuffer.str().c_str(), args ...);
    #pragma GCC diagnostic pop
    // Print the formatted string
    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__)
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__)
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__)
#define SM_ASSERT(x, msg, ...)              \
{                                           \
    if(!(x))                                \
    {                                       \
        SM_ERROR(msg, ##__VA_ARGS__);       \
        DEBUG_BREAK();                      \
        SM_ERROR("Asserting HIT!");         \
    }                                       \
}

class Player;
class Collision;
class Sprites;

extern "C" char* getTimeStamp();


bool create_file_in_data_dir(const char* filename);

bool changeFileName(std::string& path, std::string& newName);
bool loadMapConfigs(std::string& inPath);
bool fetchPlayerConfigs(Player* player);

bool fetchSpritesConfigs(Sprites *sprites);

bool saveSpriteConfig(Sprites *sprites, std::string path, int index);
bool saveSpritesConfigs(Sprites *sprites, std::string path = "");

bool fetchCollisionConfigs(Collision* collision);
bool saveCollisionConfigs(Collision* collision);
bool fetchMapConfigs(std::string& outPath);
bool fileChanged(time_t& lastWriteTime);
void removePadding(std::string& value);
