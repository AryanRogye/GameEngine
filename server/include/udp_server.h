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

#include <enet/enet.h>
#include <unordered_map>
#include "config.h"
#include <iomanip> // Add this for setw

#define PORT 8080

class UDPServer 
{
private:
    // New Enet Stuff Im Adding
    ENetHost* server;
    std::unordered_map<int, ENetPeer*> clientPeers;
    std::vector<Player> players;                        /** This is Old But I added it anyways **/
    std::mutex playersMutex; // protects access to the players list

    // Helper to print current players
    void printPlayers();
    int nextPlayerId;
public:
    bool receivedMapData = false;
    void handleCommands();
    UDPServer();
    ~UDPServer();
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
    void listenForClients();
    void handlePlayerJoined(ENetPacket* packet, ENetPeer* peer, size_t *offset);
    void addPlayer(Player player);
    void generateUnqiuePlayerId(PlayerJoined* joinedPlayer);
    void handlePacket(ENetPacket *packet, ENetPeer *peer);
    void handleMapData(ENetPacket *packet, ENetPeer *peer, size_t *offset);
    void handlePlayerMoved(ENetPacket* packet, ENetPeer* peer, size_t *offset);
    void handlePlayerSpriteIndex(ENetPacket* packet,ENetPeer* peer, size_t* offset);
    void sendAllClientsPosition(PlayerMoved movedPlayer);
    bool sendPlayerID(int playerID, ENetPeer* peer);
    bool sendMessageToClient(size_t offset, uint8_t* buffer, ENetPeer* peer);
    bool sendPlayerPosition(PlayerMoved movedPlayer,ENetPeer* peer);
    bool sendAllClientsNewPlayerJoined(PlayerJoined joinedPlayer);
    bool sendNewPlayerExistingClientInformation(PlayerJoined joinedPlayer, ENetPeer* peer);
};

