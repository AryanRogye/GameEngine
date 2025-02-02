/**
 * Purpose: This File is Used To Load The Map From The File the thing is that this is mainly
 * used for the hot reload feature
 **/

#pragma once

#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <string>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

class MapLoader
{
private:
    // Path to the map file
    std::string map_path;
    // Last TimeStamp of the file
    time_t last_timestamp;
public:
    MapLoader() = default;
    MapLoader(const std::string& map_path);
    time_t getFileTimestamp();
    void parseFile(std::vector<std::vector<int>>& mapData);
    void hotReload(std::vector<std::vector<int>>& mapData);
};

#endif
