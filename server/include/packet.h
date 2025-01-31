#pragma once

#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <pthread/qos.h>

enum PacketType 
{
    PLAYER_JOINED,
    ASSIGN_PLAYER_ID,
    PLAYER_MOVED,
};

class Serializable
{
public:
    /**
     * Serialize the packet into a buffer and return the size of the buffer
     **/
    virtual size_t serialize(uint8_t *buffer) = 0;
    /** 
     * Figure Out What the Packet Type is
     * @param buffer: The buffer that contains the packet
     * @param offset: The offset in the buffer where the packet type is located
     *  This gets updated to the next offset so that you can continue reading the rest of the packet
     **/
    static PacketType get_packet_type(const uint8_t *buffer, size_t *offset);

    /** 
     * call this after the you have identified which packet type you are dealing with 
    **/
    virtual void deserialize(const uint8_t *buffer, size_t *offset) = 0;
    virtual ~Serializable() = default;
};

class PlayerJoined : public Serializable
{
private:
    int id;
    char name[32];
public:
    void setID(int id);
    int getID() const;
    void setName(const std::string& name);
    std::string getName() const;
    PlayerJoined() = default;
    PlayerJoined(const std::string& name);
    PlayerJoined(int id, const std::string& name = "");
    size_t serialize(uint8_t *buffer) override;
    void deserialize(const uint8_t *buffer, size_t *offset) override;
};

class PlayerMoved : Serializable
{
private:
    int id;
    float x;
    float y;
public:
    PlayerMoved(int id = 0, float x = 0.0f, float y = 0.0f);
    size_t serialize(uint8_t *buffer) override;
    void deserialize(const uint8_t *buffer, size_t *offset) override;
    void    setID(int id);
    int     getID() const;
    void    setX(float x);
    float   getX() const;
    void    setY(float y);
    float   getY() const;
};

class AssignPlayerID : Serializable
{
private:
    int id;
public:
    AssignPlayerID(int id = 0);
    size_t serialize(uint8_t *buffer) override;
    void deserialize(const uint8_t *buffer, size_t *offset) override;
};

#endif
