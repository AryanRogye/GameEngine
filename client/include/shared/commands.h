#pragma once

#ifndef COMMANDS_H
#define COMMANDS_H

#include <functional>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm> // For std::transform

#ifdef _WIN32
#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#else
#include <dlfcn.h>
#endif

class World;
using CommandFunctionWithWorld = std::function<void(const std::vector<std::string_view>&, World*)>;
using CommandFunctionNoWorld = std::function<void(const std::vector<std::string_view>&)>;

class CommandSystem
{
public:
    static CommandSystem& getInstance();

    void registerCommand(const std::string& name, CommandFunctionWithWorld func);
    void registerCommand(const std::string& name, CommandFunctionNoWorld func);
    void executeCommand(std::string_view input, World* world);
    void setupCommands();
private:
    CommandSystem() = default;
    std::unordered_map<std::string, CommandFunctionWithWorld> worldCommands;
    std::unordered_map<std::string, CommandFunctionNoWorld> noWorldCommands;
};

#endif
