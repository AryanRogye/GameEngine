#include "commands.h"


void loadMapBuilder()
{
    std::cout << "Generating Map Builder...." << std::endl;
}


CommandSystem& CommandSystem::getInstance()
{
    static CommandSystem instance;
    return instance;
}

void CommandSystem::registerCommand(const std::string& name, CommandFunction func)
{
    commands[name] = std::move(func);
}

void CommandSystem::executeCommand(std::string_view input)
{
    // Trim leading spaces
    input.remove_prefix(std::min(input.find_first_not_of(' '), input.size()));

    std::vector<std::string_view> tokens;
    size_t start = 0, end = 0;
    while ((end = input.find(' ', start)) != std::string_view::npos) {
        tokens.emplace_back(input.substr(start, end - start));
        start = end + 1;
    }
    if (start < input.size()) tokens.emplace_back(input.substr(start));

    if (tokens.empty()) return;

    // Convert the command name (tokens[0]) to lowercase
    std::string commandLower(tokens[0]);
    std::transform(commandLower.begin(), commandLower.end(), commandLower.begin(), ::tolower);

    // Find the command in the map
    auto it = commands.find(commandLower);
    if (it != commands.end()) {
        it->second(tokens);
    } else {
        std::cout << "Unknown command: " << commandLower << std::endl;
    }
}

void CommandSystem::setupCommands()
{
    // Get The Current Instance Of The Command System
    CommandSystem& cmdSys = CommandSystem::getInstance();

    /** 
     * Then We Can Register Commands Into it 
     *      - DebugMode
     **/
    cmdSys.registerCommand("/mb", [](const std::vector<std::string_view>& args){
        loadMapBuilder();
    });
    cmdSys.registerCommand("/mapbuilder", [](const std::vector<std::string_view>& args){
        loadMapBuilder();
    });
}

