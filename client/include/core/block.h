/** 
 * Purpose Of this file is to define the block structure and the 
 * functions that will be used to manipulate the block
 **/

#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <unordered_map>
#include <configs.h>
#include <string>

enum class BlockType {
    GRASS,
    HORIZONTAL_DIRT,
    VERTICAL_DIRT,
    VERTICAL_LEFT_DIRT,
    INTERSECTION_DIRT,
    TREE,
    HORIZONTAL_UP_DIRT,
    VERTICAL_RIGHT_DIRT,
    HORIZONTAL_DOWN_DIRT,
    TOP_LEFT_CORNER_DIRT,
    TOP_RIGHT_CORNER_DIRT,
    BOTTOM_RIGHT_CORNER_DIRT,
    EMPTY
};

class Block {
private:
    /** This is used to store the old block type 
     *  so we dont spam the console with the same block type
     **/
    static BlockType oldBlockType;
public:
    BlockType type;
    std::string name;
    bool isSolid;

    Block(BlockType type, bool isSolid = false);

    static BlockType getBlockTypeFromString(const std::string& name);
    static std::string getBlockName(BlockType type);

    void printBlockInfo() const;
    static void printBlockInfoByPosition(int x, int y, const std::vector<std::vector<int>>& mapData);
};

#endif // BLOCK_H
