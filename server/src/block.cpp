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


Enterable::Enterable()
{
    this->isEnterable = false;
}
Enterable::Enterable(bool isEnterable)
{
    this->isEnterable = isEnterable;
}
bool Enterable::getIsEnterable() const
{
    return isEnterable;
}

Breakable::Breakable(bool isBreakable, int numHits, int multiplier)
{
    this->isBreakable = isBreakable;
    this->numHits = numHits;
    this->multiplier = multiplier;
}

Breakable::Breakable(int numHits, int multiplier)
{
    this->isBreakable = false;
    this->numHits = numHits;
    this->multiplier = multiplier;
}
bool Breakable::getIsBreakable() const {return this->isBreakable;}
int  Breakable::getNumHits() const { return this->numHits; }
int  Breakable::getMultiplier() const { return this->multiplier; }

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
    this->addBlock(BlockType::GRASS, "grass");
    this->addBlock(BlockType::HORIZONTAL_DIRT, "horizontal_dirt");
    this->addBlock(BlockType::VERTICAL_DIRT, "vertical_dirt");
    this->addBlock(BlockType::VERTICAL_LEFT_DIRT, "vertical_left_dirt");
    this->addBlock(BlockType::INTERSECTION_DIRT, "intersection_dirt");
    // Making The Tree Block Breakable
    this->addBlock<CollisionComponent>(BlockType::TREE, "tree", true);
    // We Wanna make tree breakable
    Block& treeBlock = *this->type_to_block.at(BlockType::TREE);
    this->addComponentsToBlock<Breakable>(treeBlock, true, 10, 1);

    this->addBlock(BlockType::HORIZONTAL_UP_DIRT, "horizontal_up_dirt");
    this->addBlock(BlockType::VERTICAL_RIGHT_DIRT, "vertical_right_dirt");
    this->addBlock(BlockType::HORIZONTAL_DOWN_DIRT, "horizontal_down_dirt");
    this->addBlock(BlockType::TOP_LEFT_CORNER_DIRT, "left_corner_dirt");
    this->addBlock(BlockType::TOP_RIGHT_CORNER_DIRT, "right_corner_dirt");
    this->addBlock(BlockType::BOTTOM_RIGHT_CORNER_DIRT, "bottom_right_corner_dirt");
    this->addBlock(BlockType::BOTTOM_LEFT_CORNER_DIRT, "bottom_left_corner_dirt");
    this->addBlock(BlockType::GRASS_2, "grass_2");
    // Fence blocks with collision (pass 'true' to CollisionComponent)
    this->addBlock<CollisionComponent>(BlockType::SINGLE_FENCE, "single_fence", true);
    this->addBlock<CollisionComponent>(BlockType::FENCE_ROW, "fence_row", true);
    this->addBlock<CollisionComponent>(BlockType::TOP_LEFT_CORNER_FENCE, "top_left_corner_fence", true);
    this->addBlock<CollisionComponent>(BlockType::TOP_RIGHT_CORNER_FENCE, "top_right_corner_fence", true);
    this->addBlock<CollisionComponent>(BlockType::BOTTOM_LEFT_CORNER_FENCE, "bottom_left_corner_fence", true);
    this->addBlock<CollisionComponent>(BlockType::BOTTOM_RIGHT_CORNER_FENCE, "bottom_right_corner_fence", true);
    this->addBlock<CollisionComponent>(BlockType::FENCE_COL, "fence_col", true);
    this->addBlock<CollisionComponent>(BlockType::LEFT_END_FENCE, "left_end_fence", true);
    this->addBlock<CollisionComponent>(BlockType::RIGHT_END_FENCE, "right_end_fence", true);
    // Rock Paths
    this->addBlock(BlockType::STONE_PATH_1, "stone_path_1");
    this->addBlock(BlockType::STONE_PATH_2, "stone_path_2");
    // First House
    this->addBlock<CollisionComponent>(BlockType::TOP_LEFT_HOUSE_1, "top_left_house_1", true);
    this->addBlock<CollisionComponent>(BlockType::TOP_RIGHT_HOUSE_1, "top_right_house_1", true);
    this->addBlock<CollisionComponent>(BlockType::BOTTOM_LEFT_HOUSE_1, "bottom_left_house_1", true);
    // No Collision on the bottom right house
    this->addBlock<Enterable>(BlockType::BOTTOM_RIGHT_HOUSE_1, "bottom_right_house_1", true);
    // 2nd House
    this->addBlock<CollisionComponent>(BlockType::TOP_LEFT_HOUSE_2, "top_left_house_2", true);
    this->addBlock<CollisionComponent>(BlockType::TOP_RIGHT_HOUSE_2, "top_right_house_2", true);
    this->addBlock<CollisionComponent>(BlockType::BOTTOM_LEFT_HOUSE_2, "bottom_left_house_2", true);

    this->addBlock<Enterable>(BlockType::BOTTOM_RIGHT_HOUSE_2, "bottom_right_house_2", true);

    // Adding To type_to_block
    for (auto& pair : blocks)
        this->type_to_block[pair.second.type] = &pair.second;
}


void BlockFactory::addBlock(BlockType type)
{
    std::string blockName = getBlockName(type);
    if (blocks.count(blockName)) {
        std::cout << "Block already exists: " << blockName << std::endl;
        return;
    }
    blocks.emplace(blockName, Block(type, blockName));
    type_to_block[type] = &blocks.at(blockName);
}
void BlockFactory::addBlock(BlockType type, std::string name)
{
    if (blocks.count(name)) {
        std::cout << "Block already exists: " << name << std::endl;
        return;
    }
    blocks.emplace(name, Block(type, name));
    type_to_block[type] = &blocks.at(name);
}

Block* BlockFactory::getBlockAtPosition(int x, int y, const std::vector<std::vector<int>>& mapData)
{
    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;

    if (tileY < 0 || tileY >= mapData.size() || tileX < 0 || tileX >= mapData[tileY].size()) {
        return nullptr;
    }

    int blockIndex = mapData[tileY][tileX] - 1;
    BlockType type = static_cast<BlockType>(blockIndex);
    return type_to_block.count(type) ? type_to_block.at(type) : nullptr;
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

// Initialize static variable
BlockType BlockFactory::oldBlockType = BlockType::EMPTY;
std::string BlockFactory::returnBlockInfoByPosition(int x, int y, const std::vector<std::vector<int>>& mapData)
{
    // Convert pixel pos to til idx
    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;

    if (tileY < 0 || tileY >= mapData.size() || tileX < 0 || tileX >= mapData[tileY].size()) {
        return "Player is outside map bounds!";
    }

    // Get ID of block
    int blockIndex = mapData[tileY][tileX] - 1;

    /** Convert BlockIndex to BlockType **/
    BlockType blockType = static_cast<BlockType>(blockIndex);

    for (const auto&pair : this->blocks)
    {
        if (pair.second.type == blockType)
        {
            return pair.second.name;
        }
    }
    return "unknown block";
}
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


int BlockFactory::checkBlockState(SDL_Rect rect, const std::vector<std::vector<int>>& mapData)
{
    int state = BlockState::NONE;

    int startBlockX = rect.x / TILE_SIZE;
    int endBlockX = (rect.x + rect.w) / TILE_SIZE;
    int startBlockY = rect.y / TILE_SIZE;
    int endBlockY = (rect.y + rect.h) / TILE_SIZE;

    for (int blockY = startBlockY; blockY <= endBlockY; blockY++) {
        for (int blockX = startBlockX; blockX <= endBlockX; blockX++) {
            Block* block = getBlockAtPosition(blockX * TILE_SIZE, blockY * TILE_SIZE, mapData);
            if (!block) continue;

            SDL_Rect blockRect = {
                blockX * TILE_SIZE,
                blockY * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };

            // We Will Make A Breakable BlockRect with 1.2 times the size of the block for a bigger hitbox
            double multiplier = 2;
            SDL_Rect breakableBlockRect = {
                static_cast<int>(blockX * TILE_SIZE - ((TILE_SIZE * multiplier) - TILE_SIZE) / 2),
                static_cast<int>(blockY * TILE_SIZE - ((TILE_SIZE * multiplier) - TILE_SIZE) / 2),
                static_cast<int>(TILE_SIZE * multiplier),
                static_cast<int>(TILE_SIZE * multiplier)
            };

            if (SDL_HasIntersection(&rect, &breakableBlockRect)) {
                if (block->getComponent<Breakable>() && block->getComponent<Breakable>()->getIsBreakable()) {
                    state |= BlockState::BREAKABLE;  // Set breakable flag
                }
            }
            if (SDL_HasIntersection(&rect, &blockRect)) {
                if (block->getComponent<CollisionComponent>() && block->getComponent<CollisionComponent>()->getIsSolid()) {
                    state |= BlockState::COLLISION;  // Set collision flag
                }
                if (block->getComponent<Enterable>() && block->getComponent<Enterable>()->getIsEnterable()) {
                    state |= BlockState::ENTERABLE;  // Set enterable flag
                }
            }
        }
    }
    return state;
}
