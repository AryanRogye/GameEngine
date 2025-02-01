#include <cerrno>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "packet.h"
#include <thread>
#include <cstring>
#include "player.h"
#include "vector"

#define PORT 8080

class UDPServer 
{
private:
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char buffer[1024];
    socklen_t client_addr_len;
    // Map player IDs to client addresses
    std::unordered_map<int, sockaddr_in> clientAddresses;
    std::vector<Player> players;
    int nextPlayerId;
public:
    UDPServer();
    // #########################################
    // Getters
    // #########################################
    int getSockfd() const;
    struct sockaddr_in getServerAddr() const ;
    struct sockaddr_in getClientAddr() const ;
    socklen_t getClientAddrLen() const ;
    std::vector<Player> getPlayers() const;
    int getNextPlayerId() const;
    // Getters For Client Addresses
    sockaddr_in getClientAddress(int playerID);
    std::unordered_map<int, sockaddr_in> getClientAddresses();
    // #########################################
    // Setters
    // #########################################
    void setSockfd(int sockfd) ;
    void setServerAddr(struct sockaddr_in server_addr);
    void setClientAddr(struct sockaddr_in client_addr);
    void setClientAddrLen(socklen_t client_addr_len);
    void setPlayers(std::vector<Player> players);
    void setNextPlayerId(int nextPlayerId);
    // #########################################
    // Methods
    // #########################################
    void createUDPSocket();
    void setupServerAdress();
    void bindSocket();
    void listenForClients();
    void handlePlayerJoined(uint8_t* buffer, size_t *offset, sockaddr_in clientAddr);
    void addPlayer(Player player);
    void generateUnqiuePlayerId(PlayerJoined* joinedPlayer);
    void handlePacket(std::vector<uint8_t>buffer, sockaddr_in client_addr);
    void handlePlayerMoved(uint8_t* buffer, size_t *offset);
    void sendAllClientsPosition(PlayerMoved movedPlayer);
    bool sendPlayerID(int playerID, sockaddr_in clientAddr);
    bool sendMessageToClient(size_t offset, uint8_t* buffer, sockaddr_in clientAddr);
    bool sendPlayerPosition(PlayerMoved movedPlayer,const sockaddr_in& clientAddr);
};

