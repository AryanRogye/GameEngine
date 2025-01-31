#include "client/client.h"
#include "packet.h"
#include <cstdint>
#include <cstring>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

/** Deconstructor For The Client Class **/
Client::~Client() 
{
    close(this->sockfd);
}

/** Get The Player Object **/
Player* Client::getPlayer() { return this->player; }

/** Client Constructor Main Logic For Client In Here **/
Client::Client() 
{
    this->player = new Player(0, "Player 1");
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

    // Listen for incoming Messages From The Server in a new thread
    std::thread t(&Client::listenToServer, this);
    t.detach();
}

/** 
 * This Function is For Listening To The Server
 * This Function Will Run In A Seperate Thread
 **/
void Client::listenToServer()
{
    while(true)
    {
        uint8_t buffer[1024];
        socklen_t serverAddressLength = sizeof(this->serverAddress);
        ssize_t bytesReceived = recvfrom(
            this->sockfd,
            buffer,
            1024,
            0,
            (struct sockaddr *) &this->serverAddress,
            &serverAddressLength
        );

        if (bytesReceived < 0)
        {
            std::cerr << "Error Receiving Data From The Server" << std::endl;
            continue;
        }
        this->handleRecievedPacket(buffer, bytesReceived);
    }
}

/** 
 * This Function is For Handling The Recieved Packet
 * This Function Will Figure Out What The Packet Type is
 * And Call The Correct Function To Handle The Packet
 **/
void Client::handleRecievedPacket(uint8_t* buffer,ssize_t bytesRecieved)
{
    // Figure Out What The Packet Type is
    size_t offset = 0;
    PacketType packetType =Serializable::get_packet_type(buffer, &offset);
    
    switch (packetType)
    {
        case PacketType::PLAYER_JOINED:
            break;
        case PacketType::PLAYER_MOVED:
            break;
        case PacketType::ASSIGN_PLAYER_ID:
            std::cout << "Got Player ID From Server" << std::endl;
            handleIDRecieved(buffer, bytesRecieved, &offset);
            break;
        default:
            std::cout << "Unknown Packet Type" << std::endl;
            break;
    }
}

/** 
 * This Function is For When The Server Sends Over The ID of the Player
 * This is cuz the server will assign the ID of the player based on the
 * what the server has stored in the database
 **/
void Client::handleIDRecieved(uint8_t* buffer, ssize_t bytesRecieved, size_t* offset)
{
    AssignPlayerID assignPlayerID = AssignPlayerID();
    assignPlayerID.deserialize(buffer, offset);

    // Print The ID That was Assigned To the user
    std::cout << "Player ID: " << assignPlayerID.getID() << std::endl;
    this->player->setID(assignPlayerID.getID());
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

/** 
 * This Function is For Handling When The Player Moves
 * This Function Will Send The PlayerMoved Object To The Server
 **/
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

/** 
 * This Function is For Sending A Message To The Server
 * This Function Will Send The Message To The Server
 * This is used by every other function to send data to the server
 * TODO : Change to private
 **/
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
