#include "udp_server.h"

UDPServer::UDPServer()
{
    this->nextPlayerId = 0;
    this->players = std::vector<Player>();
    this ->client_addr_len = sizeof(client_addr);
    createUDPSocket();
    setupServerAdress();
    bindSocket();
    std::thread t(&UDPServer::listenForClients, this);
    
    // This is to make sure that the main thread waits for the listenForClients thread to finish
    t.join();
}

int UDPServer::getSockfd() const { return this->sockfd; }
struct sockaddr_in UDPServer::getServerAddr() const { return server_addr; }
struct sockaddr_in UDPServer::getClientAddr() const { return client_addr; }
socklen_t UDPServer::getClientAddrLen() const { return client_addr_len; }
std::vector<Player> UDPServer::getPlayers() const { return players; }
int UDPServer::getNextPlayerId() const { return nextPlayerId; }

void UDPServer::setSockfd(int sockfd) { this->sockfd = sockfd; }
void UDPServer::setServerAddr(struct sockaddr_in server_addr) { this->server_addr = server_addr; }
void UDPServer::setClientAddr(struct sockaddr_in client_addr) { this->client_addr = client_addr;}
void UDPServer::setPlayers(std::vector<Player> players) { this->players = players; }
void UDPServer::setNextPlayerId(int nextPlayerId) { this->nextPlayerId = nextPlayerId; }
// #########################################
// Methods
// #########################################
void UDPServer::createUDPSocket()
{
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

    memset(&this->server_addr, 0, sizeof(this->server_addr));
}

void UDPServer::setupServerAdress()
{
    // Set up the server address structure
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = INADDR_ANY; 
    this->server_addr.sin_port = htons(PORT);
}

void UDPServer::bindSocket()
{
    // Bind the socket to the specified address and port
    if (bind(this->sockfd, (const struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

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
        /** 
         * Over Here Need to Deserialize the data and then
         * figure out what the packetType is
         **/
        size_t offset = 0;
        switch(Serializable::get_packet_type(reinterpret_cast<uint8_t*>(this->buffer), &offset)) 
        {
            case PacketType::PLAYER_JOINED:
                std::cout << "Player Joined" << std::endl;
                handlePlayerJoined(reinterpret_cast<uint8_t*>(buffer), &offset);
                break;
            case PacketType::PLAYER_MOVED:
                std::cout << "Player Moved" << std::endl;
                break;
            default:
                std::cout << "Unknown Packet Type" << std::endl;
                break;
        }

        /*// Send a response back to the client*/
        /*const char *msg = "Hello from server";*/
        /*sendto(this->sockfd, msg, strlen(msg), 0, (const struct sockaddr *)&this->client_addr, this->client_addr_len);*/
    }
}

void UDPServer::handlePlayerJoined(uint8_t* buffer, size_t *offset) 
{
    // Create A Player Joined Object
    PlayerJoined joinedPlayer = PlayerJoined("");
    // Deserialize the buffer and offset into the player object
    joinedPlayer.deserialize(buffer, offset);
    // Need to read the buffer and offset to get the values of the player
    // Generate A Unique ID for the player
    generateUnqiuePlayerId(&joinedPlayer);
    std::cout << "Player ID: " << joinedPlayer.get_id() << std::endl;
    std::cout << "Player Name: " << joinedPlayer.get_name() << std::endl;
}

void UDPServer::generateUnqiuePlayerId(PlayerJoined* joinedPlayer)
{
    int player_id = this->getNextPlayerId();
    this->setNextPlayerId(++player_id);
    joinedPlayer->set_id(player_id);
}

