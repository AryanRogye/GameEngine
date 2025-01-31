#include "client/client.h"
#include "packet.h"
#include <cstdint>
#include <cstring>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

Client::~Client() 
{
    close(this->sockfd);
}
Client::Client() 
{
    // Create Socket File Descriptor
    if ( (this->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
    {
        perror("Socket Creating Failed");
        exit(EXIT_FAILURE);
    }
    memset(&this->serverAddress, 0, sizeof(this->serverAddress));
    
    // Fill the server information 
    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(PORT);
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;

    int n;
    socklen_t len;
}

bool Client::handlePlayerJoined(
    std::string name
)
{
    // Create A PlayerJoined Object 
    PlayerJoined joinedPlayer = PlayerJoined(name);
    // Serialize The Player Object 
    uint8_t buffer[1024];
    uint8_t offset = joinedPlayer.serialize(buffer);
    // Send The Serialized Data To The Server
    if (!sendMessageToServer(offset, buffer)) {
        std::cout << "there Was An Error Sending The PlayerJoined Object To The Server" << std::endl;
        return false;
    }
    return false;
}

bool Client::handlePlayerMoved(
    int id,
    float x,
    float y
)
{
    // Create A Player Moved Object
    PlayerMoved movedPlayer = PlayerMoved(id, x, y);
    // Serialize The Player Object
    uint8_t buffer[1024];
    uint8_t offset = movedPlayer.serialize(buffer);
    // Send The Serialized Data To The Server
    if (!sendMessageToServer(offset, buffer)) {
        std::cout << "there Was An Error Sending The PlayerMoved Object To The Server" << std::endl;
        return false;
    }
    return true;
}

bool Client::sendMessageToServer(size_t offset, uint8_t *buffer) 
{
    if (!offset || !buffer)
    {
        return false;
    }
    ssize_t bytesSent = sendto(
        this->sockfd,
        buffer,
        offset,
        0,
        (const struct sockaddr *) &this->serverAddress, sizeof(this->serverAddress)
    );
    return bytesSent != -1;  // Fix: Proper error checking for sendto()
}
