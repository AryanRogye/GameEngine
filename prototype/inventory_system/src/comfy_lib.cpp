#include "comfy_lib.h"

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


/** 
Stored in Data/map_data.ini
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

                // Convert relative path to absolute path
                std::cout << "Value: " << value << std::endl;
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
