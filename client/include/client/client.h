#include <iostream>
#include "configs.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "packet.h"
#include <string.h>
#include <thread>
#include <mutex>
#include "player.h"

#include <enet/enet.h>

#define MAX_PLAYERS 10

// Create a UDP Socket
// Bind the socket to the server address
// Send a message to the server
// Receive a message from the server
// Close the socket
class Client
{
private:
    ENetHost* client;
    ENetPeer* peer;

    char buffer[1024];
    std::string helloMessage = "Hello from client";
    Player* player;
    std::vector<Player*> players;
    std::mutex playersMutex;
public:
    Client();
    ~Client();
    Player* getPlayer();
    const std::vector<Player*>& getPlayers();
    bool handlePlayerJoined(std::string name);
    bool handlePlayerMoved(int id, float x, float y);
    bool sendMessageToServer(size_t offset, uint8_t *buffer);
    void listenToServer();
    void handleRecievedPacket(uint8_t* buffer,ssize_t bytesRecieved);
    void handleSpriteRecieved(uint8_t* buffer,ssize_t bytesRecieved, size_t *offset);
    void handleIDRecieved(uint8_t* buffer,ssize_t bytesRecieved, size_t* offset);
    void handleNewPlayerJoined(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset);
    void handleOtherPlayersMoved(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset);
    void handleSendingMapInfo(std::vector<std::vector<int>> mapdata);

    std::vector<Player*> getPlayersSafe();
    void handleExistingPlayers(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset);
    bool handleSendingPlayerTexture(int spriteIndex);
    /*this->client->handleZombieSpawn(this->mapData.size(), this->mapData[0].size());*/
    void handleZombieSpawn(int maxRows, int maxCols);
};
