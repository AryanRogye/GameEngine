#include "comfy_lib.h"
// Needs to be included in this file to avoid circular dependencies
#include "debug_gui.h"
#include "entity/player.h"
// end of last checked includes

bool create_file_in_data_dir(const char* filename)
{
    // get the absolute path of this file
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // get directory of current file

    // first we want to verify that the ../Data/ directory exists
    std::string dataPath = basePath + "/../Data/";

    if (!std::filesystem::exists(dataPath))
    {
        return false;
    }
    
    // if we make it here then the ../Data/ directory exists
    // now we want to create the file in the directory if it doesnt exist which we will check again
    const std::string filePath = dataPath + filename;

    if (std::filesystem::exists(filePath))
    {
        return false;
    }

    // now we can create the file
    std::ofstream file(filePath);
    if (!file)
    {
        return false;
    }

    file.close();
    return true;
}

/**  
    this is assuming that the new name is a valid path name up to the last /
    and that the new name is a valid name meaning has a .png or .jpg or .jpeg
**/
bool changeFileName(std::string& path, std::string& newName)
{
    size_t lastSlash = path.find_last_of('/');

    if (lastSlash == std::string::npos)
    {
        DebugGUI::addDebugLog("Invalid path", ErrorCode::SPRITE_ERROR);
        return false;
    }

    // make sure that the new name has a valid extension
    if (newName.find(".png") == std::string::npos && 
        newName.find(".jpg") == std::string::npos && 
        newName.find(".jpeg") == std::string::npos
    ) {
        DebugGUI::addDebugLog("Invalid file extension", ErrorCode::SPRITE_ERROR);
        return false;
    }

    // now we have to make sure that this isnt already a path inside the users system
    if (std::filesystem::exists(newName))
    {
        // this means the path already exists
        DebugGUI::addDebugLog("Path already exists", ErrorCode::SPRITE_ERROR);
        return false;
    }

    // now we can rename the file
    if (std::rename(path.c_str(), newName.c_str()) != 0)
    {
        DebugGUI::addDebugLog("Failed to rename file", ErrorCode::SPRITE_ERROR);
        return false;
    }

    DebugGUI::addDebugLog("Renamed file", ErrorCode::NONE);
    return true;
}

/**
Rewriting this function in zig lol cuz i can
std::string getTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::ostringstream stream;
    stream << std::put_time(std::gmtime(&now_c), "%H:%M:%S");

    return stream.str();
}
**/

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
        DebugGUI::addDebugLog("No map_data.ini found. Using default map.", ErrorCode::MAP_ERROR);
        DebugGUI::addDebugLog("If Data/ Exists will create map_data.ini", ErrorCode::MAP_ERROR);
        create_file_in_data_dir("map_data.ini");

        return false;
    }

    configFile << "default_path='" << inPath << "'" << std::endl;

    DebugGUI::addDebugLog("Default map path written to " + configPath, ErrorCode::NONE);
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
        DebugGUI::addDebugLog("No player_data.ini found. Using default player.", ErrorCode::PLAYER_ERROR);
        create_file_in_data_dir("player_data.ini");
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
    DebugGUI::addDebugLog("Loaded Player Configs from config", ErrorCode::NONE);
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
        DebugGUI::addDebugLog("No map_data.ini found. Using default map.", ErrorCode::MAP_ERROR);
        create_file_in_data_dir("map_data.ini");

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
                DebugGUI::addDebugLog("Loaded Map Path from config", ErrorCode::NONE);
                outPath = value;
                return true;
            }
        }
    }

    outPath = basePath + "/../assets/map.json"; // Default fallback with absolute path
    loadMapConfigs(outPath);
    return true;  
}



/** 
    TODO: Bug in this function idk what it is yet i'm gonna fix the logging first
**/

bool fetchSpritesConfigs(Sprites *sprites)
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    std::string configPath = basePath + "/../Data/sprites_data.ini";

    std::ifstream configFile(configPath);
    if (!configFile)
    {
        DebugGUI::addDebugLog("No sprites_data.ini found. Using default sprites.", ErrorCode::SPRITE_ERROR);
        create_file_in_data_dir("sprites_data.ini");
        return false;
    }

    std::string line, key, value;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            if (key == "paths")
            {
                std::istringstream pathStream(value);
                std::string path;
                // Clear existing paths before loading
                sprites->clearSpritePaths();
                while (std::getline(pathStream, path, ',')) {
                    sprites->addSpritePath(path);
                }
                DebugGUI::addDebugLog("Loaded Sprite Paths from config", ErrorCode::NONE);
            }
            if (key == "numFramesX")
            {
                std::istringstream numFramesXStream(value);
                std::string path;
                int i = 0;
                while (std::getline(numFramesXStream, path, ',')) {
                    sprites->addSpriteFramesX(i, std::stoi(path));
                    i++;
                }
                DebugGUI::addDebugLog("Loaded numFramesX from config", ErrorCode::NONE);
            }
            if (key == "numFramesY")
            {
                std::istringstream numFramesYStream(value);
                std::string path;
                int i = 0;
                while (std::getline(numFramesYStream, path, ',')) {
                    sprites->addSpriteFramesY(i, std::stoi(path));
                    i++;
                }
                DebugGUI::addDebugLog("Loaded numFramesY from config", ErrorCode::NONE);
            }
        }
    }
    return true;
}

bool saveSpriteConfig(Sprites *sprites, std::string path, int index)
{
    std::string basePath = __FILE__;
    basePath = basePath.substr(0, basePath.find_last_of("/")); // Get directory of current file

    // Construct the relative path to Data/sprites_data.ini
    std::string spritePath = basePath + "/../Data/sprites_data.ini";

    std::ifstream inputFile(spritePath);
    if (!inputFile)
    {
        std::cerr << "❌ Error: Unable to open file: " << spritePath << std::endl;
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    bool foundPaths = false;

    while (std::getline(inputFile, line))
    {
        if (line.rfind("paths=", 0) == 0) // Check if the line starts with "paths="
        {
            foundPaths = true;
            std::string pathsList = line.substr(6); // Extract everything after "paths="
            std::stringstream ss(pathsList);
            std::vector<std::string> spritePaths;
            std::string sprite;

            while (std::getline(ss, sprite, ',')) // Split by comma
            {
                spritePaths.push_back(sprite);
            }

            // Make sure the index is valid
            if (index >= 0 && index < static_cast<int>(spritePaths.size()))
            {
                spritePaths[index] = path; // Update only the selected index
            }
            else if (index == static_cast<int>(spritePaths.size())) // If adding a new sprite
            {
                spritePaths.push_back(path);
            }
            else
            {
                std::cerr << "❌ Error: Invalid index " << index << std::endl;
                return false;
            }

            // Reconstruct the paths line
            std::ostringstream updatedPaths;
            updatedPaths << "paths=";
            for (size_t i = 0; i < spritePaths.size(); i++)
            {
                updatedPaths << spritePaths[i];
                if (i < spritePaths.size() - 1)
                {
                    updatedPaths << ",";
                }
            }

            lines.push_back(updatedPaths.str()); // Store the modified paths line
        }
        else
        {
            lines.push_back(line); // Keep all other lines unchanged
        }
    }
    inputFile.close();

    // If "paths=" wasn't found, we need to append it
    if (!foundPaths)
    {
        std::cerr << "⚠️ Warning: No 'paths=' section found. Creating a new one." << std::endl;
        lines.push_back("[Sprites]");
        lines.push_back("paths=" + path); // Start a new paths section
    }

    // Write back the updated content without removing other existing data
    std::ofstream outputFile(spritePath);
    if (!outputFile)
    {
        std::cerr << "❌ Error: Unable to open file for writing: " << spritePath << std::endl;
        return false;
    }

    for (const auto &l : lines)
    {
        outputFile << l << "\n";
    }

    outputFile.close();

    DebugGUI::addDebugLog("✅ Sprite path at index " + std::to_string(index) + " updated to " + path, ErrorCode::NONE);
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
        DebugGUI::addDebugLog("No collision_data.ini found. Using default collision.", ErrorCode::SPRITE_ERROR);
        std::cerr << "⚠️ No collision_data.ini found. Using default collision." << std::endl;
        return false;
    }

    if (!path.empty())
    {
        sprites->addSpritePath(path);
        DebugGUI::addDebugLog("Added Sprite Path\n" + path, ErrorCode::NONE);
    }

    // ======================================================================
    // Sprite Section Header
    // ======================================================================
    spriteFile << "[Sprites]\n";

    // List of sprite paths
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

    // ======================================================================
    // Num FramesX Section Header
    // ======================================================================
    spriteFile << "[numFramesX]\n";

    spriteFile << "numFramesX=";
    for (size_t i = 0; i < sprites->getSpritePaths().size(); i++)
    {
        // first we need to make sure that the value is not empty
        if (sprites->getSpritePaths()[i].numFramesX)
        {
            spriteFile << sprites->getSpritePaths()[i].numFramesX;
        } else {
            spriteFile << "0";
        }
        if (i < sprites->getSpritePaths().size() - 1)
        {
            spriteFile << ",";
        }
    }
    spriteFile << "\n";

    // ======================================================================
    // Num FramesY Section Header
    // ======================================================================
    spriteFile << "[numFramesY]\n";

    spriteFile << "numFramesY=";
    for (size_t i = 0; i < sprites->getSpritePaths().size(); i++)
    {
        // first we need to make sure that the value is not empty
        if (sprites->getSpritePaths()[i].numFramesY)
        {
            spriteFile << sprites->getSpritePaths()[i].numFramesY;
        } else {
            spriteFile << "0";
        }
        if (i < sprites->getSpritePaths().size() - 1)
        {
            spriteFile << ",";
        }
    }
    spriteFile << "\n";

    spriteFile.close();

    DebugGUI::addDebugLog("Collision data written to " + spritePath, ErrorCode::NONE);
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
        DebugGUI::addDebugLog("No collision_data.ini found. Using default collision.", ErrorCode::SPRITE_ERROR);
        create_file_in_data_dir("collision_data.ini");
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
                // convert string to a float
                collision->setWidth(std::stof(value));
            }
            if (key == "height")
            {
                removePadding(value);
                // convert string to a float
                collision->setHeight(std::stof(value));
            }
            if (key == "xOffset")
            {
                removePadding(value);
                // convert string to a float
                collision->setXOffset(std::stof(value));
            }
            if (key == "yOffset")
            {
                removePadding(value);
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
