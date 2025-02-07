#include "commands.h"
#include "core/world.h" // NEED THIS HERE


typedef void* (*CreateSandBoxFunc)();
typedef void (*RunSandBoxFunc)(void*);

void loadMapBuilder()
{
    std::cout << "Generating SandBox...." << std::endl;

    std::string libPath;

    #ifdef _WIN32
    libPath = "build\\sandbox.dll";
    #elif defined(__APPLE__)
    libPath = "build/libsandbox.dylib";
    #else
    libPath = "build/libsandbox.so";
    #endif
    
    // Load the shared library dynamically
    #ifdef _WIN32
    HMODULE handle = LoadLibrary(libPath.c_str());
    #else
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    #endif

    if (!handle) 
    {
        std::cerr << "(SandBox) Error: " 
        #ifdef _WIN32
        << "Failed To Load DLL!"
        #else
        << dlerror() 
        #endif
        << std::endl;
        return;
    }

    typedef void* (*CreateSandBoxFunc)();
    #ifdef _WIN32
    CreateSandBoxFunc createSandBox = (CreateSandBoxFunc)GetProcAddress(handle, "createSandBox");
    #else
    CreateSandBoxFunc createSandBox = (CreateSandBoxFunc)dlsym(handle, "createSandBox");
    #endif

    if (!createSandBox)
    {
        std::cerr << "(SandBox) Failed to find createSandBox function - Error: " 
        #ifdef _WIN32
        << "Function not found in DLL!"
        #else
        << dlerror()
        #endif
        << std::endl;
        #ifdef _WIN32
        FreeLibrary(handle);
        #else
        dlclose(handle);
        #endif
        return;
    }

    void* sandboxInstance = createSandBox();

    // Get function pointer for runSandBox.
    #ifdef _WIN32
    RunSandBoxFunc runSandBox = (RunSandBoxFunc)GetProcAddress(handle, "runSandBox");
    #else
    RunSandBoxFunc runSandBox = (RunSandBoxFunc)dlsym(handle, "runSandBox");
    #endif

    if (!runSandBox)
    {
        std::cerr << "(SandBox) Failed to find runSandBox function - Error: " 
        #ifdef _WIN32
                  << "Function not found in DLL!"
        #else
                  << dlerror()
        #endif
                  << std::endl;
        #ifdef _WIN32
        FreeLibrary(handle);
        #else
        dlclose(handle);
        #endif
        return;
    }
    
    runSandBox(sandboxInstance);

    #ifdef _WIN32
    FreeLibrary(handle);
    #else
    dlclose(handle);
    #endif
}


CommandSystem& CommandSystem::getInstance()
{
    static CommandSystem instance;
    return instance;
}

void CommandSystem::registerCommand(const std::string& name, CommandFunctionWithWorld func) {
    worldCommands[name] = std::move(func);
}

void CommandSystem::registerCommand(const std::string& name, CommandFunctionNoWorld func) {
    noWorldCommands[name] = std::move(func);
}

void CommandSystem::executeCommand(std::string_view input, World* world)
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

    // Check No World Commands
    auto it = noWorldCommands.find(commandLower);
    if (it != noWorldCommands.end()) {
        it->second(tokens);
        return;
    }

    // Check world commands
    auto itWorld = worldCommands.find(commandLower);
    if (itWorld != worldCommands.end()) {
        if (!world) {
            std::cout << "Error: This command requires a game world!" << std::endl;
            return;
        }
        itWorld->second(tokens, world);
        return;
    }

    std::cout << "Unknown command: " << tokens[0] << std::endl;
}

void CommandSystem::setupCommands()
{
    // Get The Current Instance Of The Command System
    CommandSystem& cmdSys = CommandSystem::getInstance();

    /** 
     * Then We Can Register Commands That Dont Need World Into it 
     *      - DebugMode
     **/
    cmdSys.registerCommand("/sb", [](const std::vector<std::string_view>& args){
        loadMapBuilder();
    });
    cmdSys.registerCommand("/sandbox", [](const std::vector<std::string_view>& args){
        loadMapBuilder();
    });

    /** Needs World
     *      - SetSpeed
     **/
    cmdSys.registerCommand("/set-speed", [](const std::vector<std::string_view>& args, World* world){
        if (!world)
        {
            std::cout << "Error: This command requires a game world!" << std::endl;
            return;
        }
        if (args.size() < 2)
        {
            std::cout << "Usage: /set speed <value>" << std::endl;
            return;
        }

        int newSpeed = std::stoi(std::string(args[1]));
        // ✅ Safely retrieve Client
        Client* client = world->getClient();
        if (!client) {
            std::cout << "Error: No client instance found!" << std::endl;
            return;
        }

        // ✅ Safely retrieve Player
        Player* player = client->getPlayer();
        if (!player) {
            std::cout << "Error: No player instance found!" << std::endl;
            return;
        }

        // ✅ Now it's safe to set the speed
        player->setSpeed(newSpeed);
        std::cout << "Player speed set to " << newSpeed << std::endl;
    });
}

