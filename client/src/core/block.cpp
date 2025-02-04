#include "block.h"

CollisionComponent::CollisionComponent(bool isSolid)
{
    this->isSolid = isSolid;
}
CollisionComponent::CollisionComponent()
{
    this->isSolid = false;
}
bool CollisionComponent::getIsSolid() const
{
    return isSolid;
}


/** 
 * Block Constructor Assume All Blocks Cant be walked through
 * At the start and when we add a block to the factory then
 * we can set it in there
 **/
Block::Block(BlockType type)
{
    this->type = type;
    this->name = "";
}
Block::Block(BlockType type, std::string name)
{
    this->type = type;
    this->name = name;
}
/** Read block.h for more information about both **/

BlockFactory::BlockFactory()
{
    this->blocks = std::unordered_map<std::string, Block>();

    this->blocks.emplace("grass"                        ,Block(BlockType::GRASS, "grass"));
    this->blocks.emplace("horizontal_dirt"              ,Block(BlockType::HORIZONTAL_DIRT, "horizontal_dirt"));
    this->blocks.emplace("vertical_dirt"                ,Block(BlockType::VERTICAL_DIRT, "vertical_dirt"));
    this->blocks.emplace("vertical_left_dirt"           ,Block(BlockType::VERTICAL_LEFT_DIRT, "vertical_left_dirt"));
    this->blocks.emplace("intersection_dirt"            ,Block(BlockType::INTERSECTION_DIRT, "intersection_dirt"));
    this->blocks.emplace("tree"                         ,Block(BlockType::TREE, "tree"));
    this->blocks.emplace("horizontal_up_dirt"           ,Block(BlockType::HORIZONTAL_UP_DIRT, "horizontal_up_dirt"));
    this->blocks.emplace("vertical_right_dirt"          ,Block(BlockType::VERTICAL_RIGHT_DIRT, "vertical_right_dirt"));
    this->blocks.emplace("horizontal_down_dirt"         ,Block(BlockType::HORIZONTAL_DOWN_DIRT, "horizontal_down_dirt"));
    this->blocks.emplace("left_corner_dirt"             ,Block(BlockType::TOP_LEFT_CORNER_DIRT, "left_corner_dirt"));
    this->blocks.emplace("right_corner_dirt"            ,Block(BlockType::TOP_RIGHT_CORNER_DIRT, "right_corner_dirt"));
    this->blocks.emplace("bottom_right_corner_dirt"     ,Block(BlockType::BOTTOM_RIGHT_CORNER_DIRT, "bottom_right_corner_dirt"));
    this->blocks.emplace("bottom_left_corner_dirt"      ,Block(BlockType::BOTTOM_LEFT_CORNER_DIRT, "bottom_left_corner_dirt"));
    this->blocks.emplace("house"                        ,Block(BlockType::HOUSE, "house"));
    this->blocks.emplace("single_fence"                 ,Block(BlockType::SINGLE_FENCE, "single_fence"));
    this->blocks.emplace("fence_row"                    ,Block(BlockType::FENCE_ROW, "fence_row"));
    this->blocks.emplace("right_end_fence"              ,Block(BlockType::RIGHT_END_FENCE, "right_end_fence"));
    this->blocks.emplace("left_end_fence"               ,Block(BlockType::LEFT_END_FENCE, "left_end_fence"));
    this->blocks.emplace("top_end_fence"                ,Block(BlockType::TOP_END_FENCE, "top_end_fence"));
    this->blocks.emplace("bottom_end_fence"             ,Block(BlockType::BOTTOM_END_FENCE, "bottom_end_fence"));
    this->blocks.emplace("top_left_corner_fence"        ,Block(BlockType::TOP_LEFT_CORNER_FENCE, "top_left_corner_fence"));
    this->blocks.emplace("top_right_corner_fence"       ,Block(BlockType::TOP_RIGHT_CORNER_FENCE, "top_right_corner_fence"));
    this->blocks.emplace("bottom_right_corner_fence"    ,Block(BlockType::BOTTOM_RIGHT_CORNER_FENCE, "bottom_right_corner_fence"));
    this->blocks.emplace("bottom_left_corner_fence"    ,Block(BlockType::BOTTOM_LEFT_CORNER_FENCE, "bottom_left_corner_fence"));
    this->blocks.emplace("fence_col"                   ,Block(BlockType::FENCE_COL, "fence_col"));
    this->blocks.emplace("house_big"                    ,Block(BlockType::HOUSE_BIG, "house_big"));
    this->blocks.emplace("empty"                        ,Block(BlockType::EMPTY, "empty"));

    // Set Fense Values to have collision
    this->blocks.at("single_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("fence_row").addComponent<CollisionComponent>(true);
    this->blocks.at("right_end_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("left_end_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("top_end_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("bottom_end_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("top_left_corner_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("top_right_corner_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("bottom_right_corner_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("bottom_left_corner_fence").addComponent<CollisionComponent>(true);
    this->blocks.at("fence_col").addComponent<CollisionComponent>(true);

    // Add Collision to House
    this->blocks.at("house_big").addComponent<CollisionComponent>(true);
    this->blocks.at("empty").addComponent<CollisionComponent>(true);
}



/** Convert string to BlockType **/
BlockType BlockFactory::getBlockTypeFromString(const std::string& name) 
{
    return this->blocks.at(name).type;
}

/** Convert BlockType to string **/
std::string BlockFactory::getBlockName(BlockType type) {
    for (auto& block : this->blocks) {
        if (block.second.type == type) {
            return block.second.name;
        }
    }
    return "";
}

/** Print block info **/
void BlockFactory::printBlockInfo() const {
    for (auto& block : this->blocks) {
        std::cout << "Block Name: " << block.second.name << " Block Type: " << static_cast<int>(block.second.type) << std::endl;
    }
}

// Initialize static variable
BlockType BlockFactory::oldBlockType = BlockType::EMPTY;

void BlockFactory::printBlockInfoByPosition(int x, int y, const std::vector<std::vector<int>>& mapData) 
{
    // Convert pixel pos to til idx
    int tileX = x / TILE_SIZE; 
    int tileY = y / TILE_SIZE;
    
    if (tileY < 0 || tileY >= mapData.size() || tileX < 0 || tileX >= mapData[tileY].size()) {
        std::cout << "Player is outside map bounds!" << std::endl;
        return;
    }

    // Get ID of block
    int blockIndex = mapData[tileY][tileX] - 1;

    /** 
      * Special Cases
     **/
    if (blockIndex == -1) 
    {
        if (BlockFactory::oldBlockType != BlockType::EMPTY) 
        {
            std::cout << "Player is standing on: empty" << std::endl;
            BlockFactory::oldBlockType = BlockType::EMPTY;
        }
        return;
    }

    /** House_Big **/
    if (blockIndex == -2) 
    {
        if (BlockFactory::oldBlockType != BlockType::HOUSE_BIG) 
        {
            std::cout << "Player is standing on: house_big" << std::endl;
            BlockFactory::oldBlockType = BlockType::HOUSE_BIG;
        }
        return;
    }

    /** Convert BlockIndex to BlockType **/
    BlockType blockType = static_cast<BlockType>(blockIndex);

    for (const auto&pair : this->blocks)
    {
        if (pair.second.type == blockType)
        {
            if (BlockFactory::oldBlockType != blockType)
            {
                std::cout << "Player is standing on: " << pair.second.name << std::endl;
                BlockFactory::oldBlockType = blockType;
            }
            return;
        }
    }
    std::cout << "Player is standing on: unknown block" << std::endl;
}


bool BlockFactory::checkCollision(int x, int y, const std::vector<std::vector<int>>& mapData)
{
    // Convert pixel pos to til idx
    int tileX = x / TILE_SIZE; 
    int tileY = y / TILE_SIZE;

    if (tileY < 0 || tileY >= mapData.size() || tileX < 0 || tileX >= mapData[tileY].size()) {
        std::cout << "Player is outside map bounds!" << std::endl;
        return false;
    }

    // Get ID of block
    int blockIndex = mapData[tileY][tileX] - 1;
    if (blockIndex == -1) 
    {
        if (BlockFactory::oldBlockType != BlockType::EMPTY) 
        {
            BlockFactory::oldBlockType = BlockType::EMPTY;
            return true;
        }
        return false;
    }

    /** House_Big **/
    if (blockIndex == -2) 
    {
        if (BlockFactory::oldBlockType != BlockType::HOUSE_BIG) 
        {
            BlockFactory::oldBlockType = BlockType::HOUSE_BIG;
            return true;
        }
        return false;
    }

    /** Convert BlockIndex to BlockType **/
    BlockType blockType = static_cast<BlockType>(blockIndex);

    for (const auto&pair : this->blocks)
    {
        if (pair.second.type == blockType)
        {
            if (pair.second.getComponent<CollisionComponent>() != nullptr)
            {
                CollisionComponent* collisionComponent = pair.second.getComponent<CollisionComponent>();
                if (collisionComponent == nullptr)
                {
                    std::cout << "Error: getComponent returned nullptr after checking!" << std::endl;
                    return false;
                }
                if (collisionComponent->getIsSolid())
                {
                    std::cout << "Player is colliding with: " << pair.second.name << std::endl;
                    return true;
                }
            }
        }
    }
    return false;
}
