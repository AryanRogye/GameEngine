#include "udp_server.h"
#include "packet.h"
#include <cstdint>

/** Constructor **/
UDPServer::UDPServer()
{
    // Initialize The Server Variables
    this->nextPlayerId = 0;
    this->players = std::vector<Player>();
    this ->client_addr_len = sizeof(this->client_addr);
    // Start Server Functions
    createUDPSocket();
    setupServerAdress();
    bindSocket();
    std::thread t(&UDPServer::listenForClients, this);
    // This is to make sure that the main thread waits for the listenForClients thread to finish
    t.join();
}

// Getters For the UDP Server Class
int                 UDPServer::getSockfd() const { return this->sockfd; }
struct sockaddr_in  UDPServer::getServerAddr() const { return server_addr; }
struct sockaddr_in  UDPServer::getClientAddr() const { return client_addr; }
socklen_t           UDPServer::getClientAddrLen() const { return client_addr_len; }
std::vector<Player> UDPServer::getPlayers() const { return players; }
int                 UDPServer::getNextPlayerId() const { return nextPlayerId; }

// Setters For the UDP Server Class
void UDPServer::setSockfd(int sockfd) { this->sockfd = sockfd; }
void UDPServer::setServerAddr(struct sockaddr_in server_addr) { this->server_addr = server_addr; }
void UDPServer::setClientAddr(struct sockaddr_in client_addr) { this->client_addr = client_addr;}
void UDPServer::setPlayers(std::vector<Player> players) { this->players = players; }
void UDPServer::setNextPlayerId(int nextPlayerId) { this->nextPlayerId = nextPlayerId; }
// #########################################
// Class Level Function Implementations
// #########################################

/** Create The UDP Socket **/
void UDPServer::createUDPSocket()
{
    // Create a UDP socket
    if ((this->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Allow Reuse of the port
    int opt = 1;
    setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set the socket to non-blocking mode
    int flags = fcntl(this->sockfd, F_GETFL, 0);
    fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);
    
    // Clear the server address structure
    memset(&this->server_addr, 0, sizeof(this->server_addr));
}

/** Setup The Server Address **/
void UDPServer::setupServerAdress()
{
    // Set up the server address structure
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = INADDR_ANY; 
    this->server_addr.sin_port = htons(PORT);
}

/** Bind The Socket **/
void UDPServer::bindSocket()
{
    // Bind the socket to the specified address and port
    if (bind(this->sockfd, (const struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

/** Listen For Clients **/
void UDPServer::listenForClients()
{
    std::cout << "Server listening on port " << PORT << std::endl;
    while (true) 
    {
        // Receive data from a client
        int n = recvfrom(
            this->sockfd, 
            this->buffer, 
            sizeof(this->buffer), 
            0, 
            (struct sockaddr *)&client_addr, 
            &client_addr_len
        );

        if (n < 0) 
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
            {
                // No data yet this is ok
                continue;
            } 
            else 
            {
                perror("recvfrom failed");
                exit(EXIT_FAILURE);
            }
        }
        // Create a copy of the client address
        sockaddr_in clientCopy = client_addr;
        // Handle the packet in a new thread
        std::thread(&UDPServer::handlePacket, this, std::vector<uint8_t>(buffer, buffer + n), clientCopy).detach();
    }
}

/** Handle Packet **/
void UDPServer::handlePacket(std::vector<uint8_t>packetData, sockaddr_in clientAddr)
{
    // Get The Packet Type And Handle it based on what it is
    size_t offset = 0;
    PacketType packetType = Serializable::get_packet_type(packetData.data(), &offset);


    switch (packetType) 
    {
        case PacketType::PLAYER_JOINED:
            handlePlayerJoined(packetData.data(), &offset, clientAddr);
            break;
        case PacketType::PLAYER_MOVED: // TODO <- Work in this
            handlePlayerMoved(packetData.data(), &offset);
            break;
        default:
            std::cout << "Unknown Packet Type" << std::endl;
            break;
    }
}

/** 
 * This Function will handle the movement of the player
 * will send to the other clients the position of the player
 * so that they can update the position of the player on their screen
 **/
void UDPServer::handlePlayerMoved(uint8_t* buffer, size_t *offset) 
{
    // Create A Player Moved Object
    PlayerMoved movedPlayer = PlayerMoved();
    // Deserialize the buffer and offset into the player object
    movedPlayer.deserialize(buffer, offset);
    // Need to read the buffer and offset to get the values of the player
    // Find the player in the players vector
    // Update the player's position
    // Send the updated player position to all the clients
    sendAllClientsPosition(movedPlayer);
}

/** Send All Clients Position **/
void UDPServer::sendAllClientsPosition(PlayerMoved movedPlayer)
{
    for (auto const& player : this->clientAddresses)
    {
        if (player.first != movedPlayer.getID())
        {
            // For Now Print All THe IDS were going to send the position to
            std::cout << "Sending Position to Player: " << player.first << std::endl;
            // Send The Postions To The Player
            if(!sendPlayerPosition(movedPlayer, player.second))
            {
                std::cout << "Failed To Send Position To Player: " << player.first << std::endl;
                continue;
            }
        }
    }
}

bool UDPServer::sendPlayerPosition(PlayerMoved movedPlayer,const sockaddr_in& clientAddr)
{
    // Serialize The Player Moved Enum
    uint8_t buffer[1024];
    size_t offset = movedPlayer.serialize(buffer);
    
    if (!sendMessageToClient(offset, buffer, clientAddr))
    {
        std::cout << "Failed to send player position to client" << std::endl;
        return false;
    }
    return true;
}

/** Handle Player Joined **/
void UDPServer::handlePlayerJoined(uint8_t* buffer, size_t *offset, sockaddr_in clientAddr) 
{
    // Create A Player Joined Object
    PlayerJoined joinedPlayer = PlayerJoined("");
    // Deserialize the buffer and offset into the player object
    joinedPlayer.deserialize(buffer, offset);
    // Need to read the buffer and offset to get the values of the player
    // Generate A Unique ID for the player
    generateUnqiuePlayerId(&joinedPlayer);
    // Add the player addresses to the map
    this->clientAddresses[joinedPlayer.getID()] = clientAddr;
    // Print the player's name and ID
    std::cout << "Player Joined: " << joinedPlayer.getName() << " ID: " << joinedPlayer.getID() << std::endl;
    // We Want to Send the player their assigned ID
    sendPlayerID(joinedPlayer.getID(), clientAddr);
}

/** Send Player ID **/
bool UDPServer::sendPlayerID(int playerID, sockaddr_in clientAddr)
{
    AssignPlayerID assignedID = AssignPlayerID(playerID);

    // Serialize the assignedID object
    uint8_t buffer[1024];
    uint8_t offset = assignedID.serialize(buffer);

    // Send the assignedID object to the client
    if (!sendMessageToClient(offset, buffer, clientAddr))
    {
        std::cout << "Failed to send player ID to client" << std::endl;
        return false;
    }
    return true;
}

/** Send Message To Client **/
bool UDPServer::sendMessageToClient(size_t offset, uint8_t* buffer, sockaddr_in clientAddr)
{
    if (!offset || !buffer)
    {
        return false;
    }
    // Send the buffer to the client
    ssize_t bytesSent = sendto(
        this->sockfd,          // Socket file descriptor
        buffer,                // Data to send
        offset,                // Length of data
        0,                     // Flags (0 for default)
        (struct sockaddr*)&clientAddr, // Client address
        sizeof(clientAddr)     // Size of client address
    );

    // Check if the data was sent successfully
    if (bytesSent < 0) 
    {
        std::cerr << "Error sending data to client: " << strerror(errno) << std::endl;
        return false;
    }

    // Check For Partial Data being sent
    if (bytesSent != offset)
    {
        std::cerr << "Partial Data send to client: " << bytesSent << " out of " << offset << std::endl;
    }

    return true;
}

/** Generate Unique Player ID **/
void UDPServer::generateUnqiuePlayerId(PlayerJoined* joinedPlayer)
{
    int player_id = this->getNextPlayerId();
    this->setNextPlayerId(++player_id);
    joinedPlayer->setID(player_id);
}

