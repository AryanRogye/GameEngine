

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

using CommandFunction = std::function<void(const std::vector<std::string_view>&)>;

class CommandSystem
{
public:
    static CommandSystem& getInstance();

    void registerCommand(const std::string& name, CommandFunction func);
    void executeCommand(std::string_view input);
    void setupCommands();
private:
    CommandSystem() = default;
    std::unordered_map<std::string, CommandFunction> commands;
};

#endif
