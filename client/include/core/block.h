/** 
 * Purpose Of this file is to define the block structure and the 
 * functions that will be used to manipulate the block
 **/

/**
 * Restructuring This Entire File To Follow The Design Pattern of 
 * Compostion Over Inheritance For the Block Class The reason why
 * is cuz some blocks have absolutely 0 purpose to inherit from and
 * if we do inherit itll have a bunch of useless functions that just
 * add a bunch of memory overhead and slow down the program
 **/

#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <unordered_map>
#include <configs.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <utility>


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
    BOTTOM_LEFT_CORNER_DIRT,
    HOUSE,
    SINGLE_FENCE,
    FENCE_ROW,
    RIGHT_END_FENCE,
    LEFT_END_FENCE,
    TOP_END_FENCE,
    BOTTOM_END_FENCE,
    TOP_LEFT_CORNER_FENCE,
    TOP_RIGHT_CORNER_FENCE,
    BOTTOM_RIGHT_CORNER_FENCE,
    BOTTOM_LEFT_CORNER_FENCE,
    FENCE_COL,
    HOUSE_BIG,
    EMPTY
};
class BlockComponent {
public:
    BlockComponent() = default;
    virtual ~BlockComponent() = default;
};

class CollisionComponent : public BlockComponent {
    bool isSolid;
public:
    CollisionComponent(bool isSolid);
    CollisionComponent();
    bool getIsSolid() const;
};

class Enterable : public BlockComponent {
    bool isEnterable;
public:
    Enterable(bool isEnterable);
    Enterable();
    bool getIsEnterable() const;
};

class Block {
public:

    BlockType type;
    std::string name;
    std::vector<std::unique_ptr<BlockComponent>> components;
    /** Functions **/
    Block(BlockType type);
    Block(BlockType type, std::string name);
    /** 
     * This Looks Very Complicated byt its very simple
     * this is a variadic template function that takes in a type T
     * and a bunch of arguments and creates a new instance of T and
     * adds it to the components vector

     * The Components that can be used are right now:
     *       - CollisionComponent
     * Example : addComponent<CollisionComponent>(true);
     **/
    /** Add Components **/
    template <typename T, typename... Args>
    void addComponent(Args&&... args) {
        if (getComponent<T>()) {
            std::cout << "Component already exists in block: " << this->name << std::endl;
            return;
        }
        components.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }
    
    /** 
     * This Function is a getter function 
     **/
    template <typename T>
    T* getComponent() const
    {
        for (auto& comp : components) 
        {
            if(typeid(*comp) == typeid(T))
                return static_cast<T*>(comp.get());
        }
        return nullptr;
    }
};

/** 
 * Purpose of this class is to create a bunch of blocks
 * which is kind of like a "Factory" Design Pattern
 **/
class BlockFactory {
    std::unordered_map<std::string, Block> blocks;
    std::unordered_map<BlockType, Block*> type_to_block;
    static BlockType oldBlockType;
public:
    BlockFactory();
    template <typename T, typename... Args>
    void addComponentToBlock(BlockType type, Args&&... args) 
    {
        if (type_to_block.count(type)) 
        {
            type_to_block.at(type)->addComponent<T>(std::forward<Args>(args)...);
        }
    }
    void addBlock(BlockType type, std::string name);
    void addBlock(BlockType type);
    template <typename... Components, typename... Args>
    void addBlock(BlockType type, const std::string& name, Args&&... args) {
        if (blocks.count(name)) {
            std::cout << "Block already exists: " << name << std::endl;
            return;
        }
        blocks.emplace(name, Block(type, name));
        type_to_block.emplace(type, &blocks.at(name));
        addComponentsToBlock<Components...>(blocks.at(name), std::forward<Args>(args)...);
    }

    template <typename T, typename... Rest, typename... Args>
    void addComponentsToBlock(Block& block, Args&&... args) {
        block.addComponent<T>(std::forward<Args>(args)...);
        if constexpr (sizeof...(Rest) > 0) {
            addComponentsToBlock<Rest...>(block, std::forward<Args>(args)...);
        }
    }
    
    Block* getBlockAtPosition(int x, int y, const std::vector<std::vector<int>>& mapData);
    BlockType getBlockTypeFromString(const std::string& name);
    std::string getBlockName(BlockType type);
    void printBlockInfo() const;
    void printBlockInfoByPosition(int x, int y, const std::vector<std::vector<int>>& mapData);
    bool checkCollision(int x, int y, const std::vector<std::vector<int>>& mapData);
    bool checkEnterable(int x, int y, const std::vector<std::vector<int>>& mapData);
};

#endif // BLOCK_H
