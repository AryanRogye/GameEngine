#include "map_loader.h"

MapLoader::MapLoader(const std::string& map_path) : map_path(map_path), last_timestamp(0) {}

time_t MapLoader::getFileTimestamp()
{
    struct stat fileInfo;
    if (stat(this->map_path.c_str(), &fileInfo) == 0) {
        return fileInfo.st_mtime;
    }
    return 0; // Default value if file not found
}

void MapLoader::parseFile(std::vector<std::vector<int>>& mapData)
{
    std::ifstream file(this->map_path);
    if (!file.is_open())
    {
        std::cout << "Failed to open file" << std::endl;
        return;
    }
    // Clear the map data

    mapData.clear();
    std::string line;
    while (std::getline(file, line))
    {
        std::vector<int> row;
        std::istringstream iss(line);
        std::string cell;

        // Read each hex value (separated by whitespace)
        while (iss >> cell)
        {
            try {
                // Convert the hex string to an integer
                int cellValue = std::stoi(cell, nullptr, 16);
                std::cout << "Read cell value: " << cellValue << std::endl;
                row.push_back(cellValue);
            }
            catch(const std::exception &e) {
                std::cerr << "Error converting '" << cell << "': " << e.what() << std::endl;
            }
        }
        mapData.push_back(row);
    }
    file.close();
    this->last_timestamp = getFileTimestamp();
}

void MapLoader::hotReload(std::vector<std::vector<int>>& mapData)
{
    if (this->getFileTimestamp() > this->last_timestamp) {
        std::cout << "Reloading map due to changes detected!" << std::endl;
        this->parseFile(mapData);
    }
}
