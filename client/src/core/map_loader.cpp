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

bool MapLoader::saveFile(const std::vector<std::vector<int>>& mapData)
{
    std::ofstream file(this->map_path);
    if (!file.is_open())
    {
        std::cout << "Failed to open file" << std::endl;
        return false;
    }

    for (const auto& row : mapData)
    {
        for (size_t i = 0; i < row.size(); i++)
        {
            if (row[i] == 0)
            {
                file << "MM";
            }
            else if (row[i] == -1)
            {
                file << "HH";
            } else {
                // Convert to hex, ensuring 2-digit formatting (01, 02, 0A, 0F, etc.)
                file << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << row[i];
            }
            // Add spacing except at the last element in the row
            if (i < row.size() - 1)
            {
                file << "  ";
            }
        }
        file << "\n"; // New line after each row
    }

    file.close();
    return true;
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
            try 
            {
                // Convert the hex string to an integer
                int cellValue = std::stoi(cell, nullptr, 16);
                row.push_back(cellValue);
            }
            catch(const std::exception &e) 
            {
                if (cell == "MM" || cell == "mm") 
                {
                    row.push_back(0);
                    continue;
                }
                if (cell == "HH" || cell == "hh") 
                {
                    row.push_back(-1);
                    continue;
                }
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
    if (this->getFileTimestamp() > this->last_timestamp) 
    {
        std::cout << "Reloading map due to changes detected!" << std::endl;
        this->parseFile(mapData);
    }
}
