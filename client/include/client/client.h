#include <iostream>
#include "configs.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "packet.h"
#include <string.h>
#include "thread"

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
public:
    Client();
    ~Client();
    bool handlePlayerJoined(std::string name);
    bool handlePlayerMoved();
    bool sendMessageToServer(size_t offset, uint8_t *buffer);
};
