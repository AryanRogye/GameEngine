#include "comfy_lib.h"
// Needs to be included in this file to avoid circular dependencies
#include "entity/player.h"
// end of last checked includes

std::string getTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string timeString = std::ctime(&now_c);
    timeString.pop_back(); // remove the newline character
    return timeString;
}



/** 
Will write to the Data/map_data.ini file
a default_path
**/
bool loadMapConfigs(std::string& inPath)
{
    // Get the absolute path of this file
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    std::cout << "basePath: " << basePath << std::endl;

    // Construct the relative path to Data/map_data.ini
    std::string configPath = basePath + "/../Data/map_data.ini";

    std::ofstream configFile(configPath);
    if (!configFile)
    {
        std::cerr << "⚠️ No map_data.ini found. Using default map." << std::endl;
        return false;
    }

    configFile << "default_path='" << inPath << "'" << std::endl;

    std::cout << "Default map path written to " << configPath << std::endl;
    return true;
}

void removePadding(std::string& value)
{
    // Remove surrounding quotes if present
    if (!value.empty() && value.front() == '\'' && value.back() == '\'')
    {
        value = value.substr(1, value.size() - 2);
    }
}

/**
This Function will load in values from the Data/player_data.ini file
**/
bool fetchPlayerConfigs(Player *player) // not sure how i want to get these values
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    // Construct the relative path to Data/player_data.ini
    std::string configPath = basePath + "/../Data/player_data.ini";

    std::ifstream configFile(configPath);
    if (!configFile)
    {
        std::cerr << "⚠️ No player_data.ini found. Using default player." << std::endl;
        return false;
    }

    std::string line, key, value;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            // =======================================================================================================
            // Player Acceleration
            // =======================================================================================================
            if (key == "acceleration") 
            {
                removePadding(value);
                // convert string to a float
                player->setAcceleration(std::stof(value));
            }
            // =======================================================================================================
            // Player Max Speed
            // =======================================================================================================
            if (key == "maxSpeed") 
            {
                removePadding(value);

                // convert string to a float
                player->setMaxSpeed(std::stof(value));
            }
            // =======================================================================================================
            // Player Friction
            // =======================================================================================================
            if (key == "friction") 
            {
                removePadding(value);

                // convert string to a float
                player->setFriction(std::stof(value));
            }
            // =======================================================================================================
            // Player Health
            // =======================================================================================================
            if (key == "health") 
            {
                removePadding(value);

                // convert string to a int
                player->setHealth(std::stoi(value));
            }
            // =======================================================================================================
            // Player Max Health
            // =======================================================================================================
            if (key == "maxHealth") 
            {
                removePadding(value);

                // convert string to a int
                player->setMaxHealth(std::stoi(value));
            }
            // =======================================================================================================
            // Player Damage
            // =======================================================================================================
            if (key == "damage") 
            {
                removePadding(value);

                // convert string to a int
                player->setDamage(std::stoi(value));
            }
            // =======================================================================================================
            // Player Level
            // =======================================================================================================
            if (key == "level") 
            {
                removePadding(value);

                // convert string to a int
                player->setLevel(std::stoi(value));
            }
            // =======================================================================================================
            // Player Experience
            // =======================================================================================================
            if (key == "experience") 
            {
                removePadding(value);

                // convert string to a int
                player->setExperience(std::stoi(value));
            }
            // =======================================================================================================
            // Player X Position
            // =======================================================================================================
            if (key == "x") 
            {
                removePadding(value);
                
                // convert string to a float
                player->setX(std::stof(value));
            }
            // =======================================================================================================
            // Player Y Position
            // =======================================================================================================
            if (key == "y") 
            {
                removePadding(value);

                // convert string to a float
                player->setY(std::stof(value));
            }
            // =======================================================================================================
            // Player Velocity X
            // =======================================================================================================
            if (key == "velocityX") 
            {
                removePadding(value);
                
                // convert string to a float
                player->setVelocityX(std::stof(value));
            }
            // =======================================================================================================
            // Player Velocity Y
            // =======================================================================================================
            if (key == "velocityY") 
            {
                removePadding(value);

                // convert string to a float
                player->setVelocityY(std::stof(value));
            }
        }
    }
    return true;
}

/** 
This Function will load in values from the Data/map_data.ini file
**/
bool fetchMapConfigs(std::string& outPath) 
{
    // Get the absolute path of this file
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    // Construct the relative path to Data/map_data.ini
    std::string configPath = basePath + "/../Data/map_data.ini";

    std::ifstream configFile(configPath);
    if (!configFile)
    {
        std::cerr << "⚠️ No map_data.ini found. Using default map." << std::endl;
        outPath = basePath + "/../assets/map.json";  // Make sure default also uses absolute path
        loadMapConfigs(outPath);
        return false;
    }

    std::string line, key, value;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            if (key == "default_path")
            {
                // Remove surrounding quotes if present
                if (!value.empty() && value.front() == '\'' && value.back() == '\'')
                {
                    value = value.substr(1, value.size() - 2);
                }

                if (value.empty())
                {
                    // We Return False but tbh theres no real problem with it being empty
                    return false;
                }

                if (value.front() != '/')
                {
                    value = basePath + "/../" + value;
                }
                std::cout << "Absolute path: " << value << std::endl;
                outPath = value;
                return true;
            }
        }
    }

    outPath = basePath + "/../assets/map.json"; // Default fallback with absolute path
    loadMapConfigs(outPath);
    return true;  
}

bool fetchSpritesConfigs(Sprites *sprites)
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    std::string configPath = basePath + "/../Data/sprites_data.ini";

    std::ifstream configFile(configPath);
    if (!configFile)
    {
        std::cerr << "⚠️ No sprites_data.ini found. Using default sprites." << std::endl;
        return false;
    }

    std::string line, key, value;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            // =======================================================================================================
            // Player Acceleration
            // =======================================================================================================
            if (key == "paths")
            {
              std::istringstream pathStream(value);
              std::string path;
              sprites->clearSpritePaths(); // Clear existing paths before loading
              while (std::getline(pathStream, path, ',')) {
                sprites->addSpritePath(path);
              }
              DebugGUI::addDebugLog("Loaded Sprite Paths from config", false,
                                    "SPRITE");
            }
        }
    }
    return true;
}

bool saveSpritesConfigs(Sprites *sprites, std::string path)
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    // Construct the relative path to Data/collision_data.ini
    std::string spritePath = basePath + "/../Data/sprites_data.ini";

    std::ofstream spriteFile(spritePath);
    if (!spriteFile)
    {
        std::cerr << "⚠️ No collision_data.ini found. Using default collision." << std::endl;
        return false;
    }

    // Write the section header
    spriteFile << "[Sprites]\n";

    // if path isnt emtpy we wanna add it to the vector of sprite paths
    if (!path.empty())
    {
        sprites->addSpritePath(path);
        DebugGUI::addDebugLog("Added Sprite Path\n" + path, false, "SPRITE");
    }

    // Write each sprite path to the file
    spriteFile << "paths=";
    for (size_t i = 0; i < sprites->getSpritePaths().size(); i++)
    {
        spriteFile << sprites->getSpritePaths()[i].path;
        if (i < sprites->getSpritePaths().size() - 1)
        {
            spriteFile << ","; // Separate paths with commas
        }
    }
    spriteFile << "\n";

    spriteFile.close();

    std::cout << "Collision data written to " << spritePath << std::endl;
    return true;
}

bool fetchCollisionConfigs(Collision *collision)
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    std::string configPath = basePath + "/../Data/collision_data.ini";

    std::ifstream configFile(configPath);
    if (!configFile)
    {
        std::cerr << "⚠️ No collision_data.ini found. Using default collision." << std::endl;
        return false;
    }

    std::string line, key, value;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            // =======================================================================================================
            // Player Acceleration
            // =======================================================================================================
            if (key == "width")
            {
                removePadding(value);
                std::cout << "Value: " << value << std::endl;
                // convert string to a float
                collision->setWidth(std::stof(value));
            }
            if (key == "height")
            {
                removePadding(value);
                std::cout << "Value: " << value << std::endl;
                // convert string to a float
                collision->setHeight(std::stof(value));
            }
            if (key == "xOffset")
            {
                removePadding(value);
                std::cout << "Value: " << value << std::endl;
                // convert string to a float
                collision->setXOffset(std::stof(value));
            }
            if (key == "yOffset")
            {
                removePadding(value);
                std::cout << "Value: " << value << std::endl;
                // convert string to a float
                collision->setYOffset(std::stof(value));
            }
        }
    }
    return true;
}





/** 
Detect changes in file timestamps of the map_data.ini
**/
bool fileChanged(time_t& lastWriteTime)
{
    // Get the absolute path of this file
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    // Construct the relative path to Data/map_data.ini
    std::string configPath = basePath + "/../Data/map_data.ini";

    struct stat result;
    if (stat(configPath.c_str(), &result) == 0)
    {
        if (result.st_mtime != lastWriteTime)
        {
            lastWriteTime = result.st_mtime;
            return true;
        }
    }
    return false;
}

/**
    Will write to the Data/collision_data.ini file
**/
bool saveCollisionConfigs(Collision* collision)
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    // Construct the relative path to Data/collision_data.ini
    std::string configPath = basePath + "/../Data/collision_data.ini";

    std::ofstream configFile(configPath);
    if (!configFile)
    {
        std::cerr << "⚠️ No collision_data.ini found. Using default collision." << std::endl;
        return false;
    }

    configFile << "width='" << collision->getWidth() << "'" << std::endl;
    configFile << "height='" << collision->getHeight() << "'" << std::endl;
    configFile << "xOffset='" << collision->getXOffset() << "'" << std::endl;
    configFile << "yOffset='" << collision->getYOffset() << "'" << std::endl;

    std::cout << "Collision data written to " << configPath << std::endl;
    return true;
}
