#include "block.h"
#include <iostream>

const std::unordered_map<std::string, BlockType> BLOCK_NAME_MAP = 
{
        {"grass", BlockType::GRASS},
        {"horizontal_dirt", BlockType::HORIZONTAL_DIRT},
        {"vertical_dirt", BlockType::VERTICAL_DIRT},
        {"vertical_left_dirt", BlockType::VERTICAL_LEFT_DIRT},
        {"intersection_dirt", BlockType::INTERSECTION_DIRT},
        {"tree", BlockType::TREE},
        {"horizontal_up_dirt", BlockType::HORIZONTAL_UP_DIRT},
        {"vertical_right_dirt", BlockType::VERTICAL_RIGHT_DIRT},
        {"horizontal_down_dirt",BlockType::HORIZONTAL_DOWN_DIRT },
        {"left_corner_dirt",BlockType::TOP_LEFT_CORNER_DIRT },
        {"right_corner_dirt",BlockType::TOP_RIGHT_CORNER_DIRT },
        {"bottom_right_corner_dirt", BlockType::BOTTOM_RIGHT_CORNER_DIRT},
        {"bottom_left_corner_dirt" , BlockType::BOTTOM_LEFT_CORNER_DIRT},
        {"house", BlockType::HOUSE },
        {"single_fence",BlockType::SINGLE_FENCE},
        {"fence_row",BlockType::FENCE_ROW},
        {"right_end_fence",BlockType::RIGHT_END_FENCE},
        {"left_end_fence",BlockType::LEFT_END_FENCE},
        {"top_end_fence",BlockType::TOP_END_FENCE},
        {"bottom_end_fence",BlockType::BOTTOM_END_FENCE},
        {"top_left_corner_fence",BlockType::TOP_LEFT_CORNER_FENCE},
        {"top_right_corner_fence",BlockType::TOP_RIGHT_CORNER_FENCE},
        {"bottom_right_corner_fence",BlockType::BOTTOM_RIGHT_CORNER_FENCE},
        {"bottom_left_corner_fence",BlockType::BOTTOM_LEFT_CORNER_FENCE},
        {"fence_col",BlockType::FENCE_COL},
};

const std::unordered_map<BlockType, std::string> BLOCK_TYPE_NAMES = 
{
        {BlockType::GRASS, "grass"},
        {BlockType::HORIZONTAL_DIRT, "horizontal_dirt"},
        {BlockType::VERTICAL_DIRT, "vertical_dirt"},
        {BlockType::VERTICAL_LEFT_DIRT, "vertical_left_dirt"},
        {BlockType::INTERSECTION_DIRT, "intersection_dirt"},
        {BlockType::TREE, "tree"},
        {BlockType::HORIZONTAL_UP_DIRT, "horizontal_up_dirt"},
        {BlockType::VERTICAL_RIGHT_DIRT, "vertical_right_dirt"},
        {BlockType::HORIZONTAL_DOWN_DIRT, "horizontal_down_dirt"},
        {BlockType::TOP_LEFT_CORNER_DIRT, "left_corner_dirt"},
        {BlockType::TOP_RIGHT_CORNER_DIRT, "right_corner_dirt"},
        {BlockType::BOTTOM_RIGHT_CORNER_DIRT, "bottom_right_corner_dirt"},
        {BlockType::HOUSE, "house"},
        {BlockType::SINGLE_FENCE, "single_fence"},
        {BlockType::FENCE_ROW, "fence_row"},
        {BlockType::RIGHT_END_FENCE, "right_end_fence"},
        {BlockType::LEFT_END_FENCE, "left_end_fence"},
        {BlockType::TOP_END_FENCE,"top_end_fence"},
        {BlockType::BOTTOM_END_FENCE, "bottom_end_fence"},
        {BlockType::TOP_LEFT_CORNER_FENCE, "top_left_corner_fence"},
        {BlockType::TOP_RIGHT_CORNER_FENCE, "top_right_corner_fence"},
        {BlockType::BOTTOM_RIGHT_CORNER_FENCE, "bottom_right_corner_fence"},
        {BlockType::BOTTOM_LEFT_CORNER_FENCE, "bottom_left_corner_fence"},
        {BlockType::FENCE_COL, "fence_col"},
};

Block::Block(BlockType type, bool isSolid) : type(type), isSolid(isSolid) 
{
    name = BLOCK_TYPE_NAMES.at(type);
}

/** Convert string to BlockType **/
BlockType Block::getBlockTypeFromString(const std::string& name) 
{
    auto it = BLOCK_NAME_MAP.find(name);
    return (it != BLOCK_NAME_MAP.end()) ? it->second : BlockType::EMPTY;
}

/** Convert BlockType to string **/
std::string Block::getBlockName(BlockType type) {
    auto it = BLOCK_TYPE_NAMES.find(type);
    return (it != BLOCK_TYPE_NAMES.end()) ? it->second : "unknown";
}

/** Print block info **/
void Block::printBlockInfo() const {
    std::cout << "Block: " << name << ", Solid: " << (isSolid ? "Yes" : "No") << std::endl;
}

// Initialize static variable
BlockType Block::oldBlockType = BlockType::EMPTY;

void Block::printBlockInfoByPosition(int x, int y, const std::vector<std::vector<int>>& mapData) 
{
    int tileX = x / TILE_SIZE; // Convert pixel position to tile index
    int tileY = y / TILE_SIZE;
    
    // Block Type is 0 indexed map data is 1 indexed
    if (tileY >= 0 && tileY < mapData.size() && tileX >= 0 && tileX < mapData[tileY].size()) {
        int blockIndex = mapData[tileY][tileX] - 1; // Get block ID from map
        BlockType blockType = static_cast<BlockType>(blockIndex); // Convert to enum

        // ✅ Only print if the block type changed
        if (blockType != Block::oldBlockType) {
            if (BLOCK_TYPE_NAMES.find(blockType) != BLOCK_TYPE_NAMES.end()) {
                std::cout << "Player is standing on: " << BLOCK_TYPE_NAMES.at(blockType) << std::endl;
            } else {
                std::cout << "Player is standing on an unknown block." << std::endl;
            }

            // ✅ Update oldBlockType to avoid duplicate messages
            Block::oldBlockType = blockType;
        }
    } else {
        std::cout << "Player is outside map bounds!" << std::endl;
    }
}
