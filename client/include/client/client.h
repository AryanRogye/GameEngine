#include <iostream>
#include "configs.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "packet.h"
#include <string.h>
#include "thread"
#include "player.h"

#define MAX_PLAYERS 10

// Create a UDP Socket
// Bind the socket to the server address
// Send a message to the server
// Receive a message from the server
// Close the socket
class Client
{
private:
    int sockfd;
    char buffer[1024];
    struct sockaddr_in serverAddress;
    std::string helloMessage = "Hello from client";
    Player* player;
    std::vector<Player*> players;
public:
    Client();
    ~Client();
    Player* getPlayer();
    bool handlePlayerJoined(std::string name);
    bool handlePlayerMoved(int id, float x, float y);
    bool sendMessageToServer(size_t offset, uint8_t *buffer);
    void listenToServer();
    void handleRecievedPacket(uint8_t* buffer,ssize_t bytesRecieved);
    void handleIDRecieved(uint8_t* buffer,ssize_t bytesRecieved, size_t* offset);
    void handleNewPlayerJoined(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset);
};
