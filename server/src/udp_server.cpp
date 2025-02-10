#include "udp_server.h"

UDPServer::~UDPServer()
{
    enet_host_destroy(server);
    enet_deinitialize();
}
/** Constructor **/
UDPServer::UDPServer()
{
        
    if (enet_initialize() != 0)
    {
        std::cerr << "An error occurred while initializing ENet." << std::endl;
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = PORT;
    
    /** This is  **/
    this->server = enet_host_create(
        &address,       // The address to bind the server host to
        MAX_CLIENTS,    // Allow up to 10 clients and/or outgoing connections
        2,              // Allow up to 2 channels to be used, 0 and 1
        0,              // Assume any amount of incoming bandwidth
        0               // Assume any amount of outgoing bandwidth
    );
    if (!this->server)
    {
        std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << PORT << std::endl;

    /*this->nextPlayerId = 0;*/
    /*this->players = std::vector<Player>();*/

    this->listenForClients();
}

// Getters For the UDP Server Class
std::vector<Player> UDPServer::getPlayers() const { return players; }
int                 UDPServer::getNextPlayerId() const { return nextPlayerId; }

// Setters For the UDP Server Class
void UDPServer::setPlayers(std::vector<Player> players) { this->players = players; }
void UDPServer::setNextPlayerId(int nextPlayerId) { this->nextPlayerId = nextPlayerId; }
// #########################################
// Class Level Function Implementations
// #########################################


/** Listen For Clients **/
void UDPServer::listenForClients()
{
    ENetEvent event;
    
    while (true)
    {
        while (enet_host_service(server, &event, 1000) > 0) 
        {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "A new client connected!\n";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    std::cout << "Received packet of size: " << event.packet->dataLength << std::endl;
                    handlePacket(event.packet, event.peer);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected.\n";
                    break;
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }
}

/** Handle Packet **/
void UDPServer::handlePacket(ENetPacket *packet, ENetPeer *peer)
{
    size_t offset = 0;
    PacketType packetType = Serializable::get_packet_type(packet->data, &offset);

    switch (packetType) 
    {
        case PacketType::PLAYER_JOINED:
            handlePlayerJoined(packet,peer,&offset);
            break;
        case PacketType::PLAYER_MOVED:
            handlePlayerMoved(packet, peer, &offset);
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
/** TODO FIX  **/
void UDPServer::handlePlayerMoved(ENetPacket* packet, ENetPeer* peer, size_t* offset)
{
    // Create A Player Moved Object
    PlayerMoved movedPlayer = PlayerMoved();
    // Deserialize the buffer and offset into the player object
    movedPlayer.deserialize(packet->data, offset);
    // Need to read the buffer and offset to get the values of the player
    // Find the player in the players vector
    // Update the player's position
    // Send the updated player position to all the clients
    sendAllClientsPosition(movedPlayer);
}

/** Send All Clients Position **/
void UDPServer::sendAllClientsPosition(PlayerMoved movedPlayer)
{
    for (auto const& player : this->clientPeers)
    {
        if (player.first != movedPlayer.getID())
        {
            // Send The Postions To The Player
            if(!sendPlayerPosition(movedPlayer, player.second))
            {
                std::cout << "Failed To Send Position To Player: " << player.first << std::endl;
                continue;
            }
        }
    }
}

bool UDPServer::sendPlayerPosition(PlayerMoved movedPlayer,ENetPeer* peer)
{
    // Serialize The Player Moved Enum
    uint8_t buffer[1024];
    size_t offset = movedPlayer.serialize(buffer);
    
    if (!sendMessageToClient(offset,buffer, peer))
    {
        std::cout << "Failed to send player position to client" << std::endl;
        return false;
    }
    return true;
}

/** Handle Player Joined **/
void UDPServer::handlePlayerJoined(ENetPacket* packet, ENetPeer* peer, size_t *offset) 
{
    // Create A Player Joined Object
    PlayerJoined joinedPlayer = PlayerJoined("");
    // Deserialize the buffer and offset into the player object
    joinedPlayer.deserialize(packet->data, offset);
    // Need to read the buffer and offset to get the values of the player
    // Generate A Unique ID for the player
    generateUnqiuePlayerId(&joinedPlayer);
    // Add the player addresses to the map
    this->clientPeers[joinedPlayer.getID()] = peer;
    // Add the plaeyr to the players vector
    // Print the player's name and ID
    std::cout << "Player Joined: " << joinedPlayer.getName() << " ID: " << joinedPlayer.getID() << std::endl;
    // We Want to Send the player their assigned ID
    sendPlayerID(joinedPlayer.getID(), peer);       /** TODO Need to Fix This **/
    Player player_to_add = Player(joinedPlayer.getID(), joinedPlayer.getName().c_str());
    this->players.push_back(player_to_add);

    // If Other Players Exist
    if (this->clientPeers.size() > 1)
    {
        // First Step is To Tell All The Current Users That a New Player Has Joined
        sendAllClientsNewPlayerJoined(joinedPlayer);
        // Second Step is to send the new player the information about the other players
        sendNewPlayerExistingClientInformation(joinedPlayer, peer);
    } 
    else 
    {
        std::cout << "Your The First One Here" << std::endl;
    }

}

/** 
 * This Function Sends The New Client Information about the existing clients
 **/
bool UDPServer::sendNewPlayerExistingClientInformation(PlayerJoined joinedPlayer, ENetPeer* peer)
{
    std::vector<Player> playersInGame;
    for (Player& player : this->players)
    {
        if (player.getID() == joinedPlayer.getID()) continue;
        playersInGame.push_back(player);
    }

    SendExistingClientsToNewPlayer sendExistingClients = SendExistingClientsToNewPlayer(playersInGame);
    uint8_t buffer[1024];

    size_t offset = sendExistingClients.serialize(buffer);

    if(!sendMessageToClient(offset, buffer, peer))
    {
        std::cout << "Failed to send existing clients to new player" << std::endl;
        return false;
    }
    return true;
}

/** 
 * This Function Sends All The Clients that are currently in the game
 * information about the new client that just joined
 **/
bool UDPServer::sendAllClientsNewPlayerJoined(PlayerJoined joinedPlayer) // TODO: Gonna take a break <- Come Back To This Spot
{
    SendToExisitingClientsNewPlayer sendExistingClients = SendToExisitingClientsNewPlayer(joinedPlayer.getID());
    uint8_t buffer[1024];
    size_t offset = sendExistingClients.serialize(buffer);
    // Now we need to send the information to all the clients
    for (auto const& player : this->clientPeers)
    {
        if (player.first != joinedPlayer.getID())
        {
            if(!sendMessageToClient(offset, buffer, player.second))
            {
                std::cout << "Failed to send new player to existing clients" << std::endl;
                return false;
            }
        }
    }
    return true;
}


/** Send Player ID **/
bool UDPServer::sendPlayerID(int playerID, ENetPeer* peer)
{
    AssignPlayerID assignedID = AssignPlayerID(playerID);

    // Serialize the assignedID object
    uint8_t buffer[1024];
    size_t offset = assignedID.serialize(buffer);

    // Send the assignedID object to the client
    if (!sendMessageToClient(offset, buffer, peer))
    {
        std::cout << "Failed to send player ID to client" << std::endl;
        return false;
    }
    return true;
}

bool UDPServer::sendMessageToClient(size_t offset, uint8_t* buffer, ENetPeer* peer)
{
    if (!offset || !buffer || !peer) 
    {
        return false;
    }
    // Create an ENet packet
    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    if (!packet) 
    {
        std::cerr << "Failed to create ENet packet.\n";
        return false;
    }
    // Send packet to client via ENet peer
    enet_peer_send(peer, 0, packet);
    
    // Ensure the data is sent immediately
    enet_host_flush(peer->host);

    return true;
}

/** Generate Unique Player ID **/
void UDPServer::generateUnqiuePlayerId(PlayerJoined* joinedPlayer)
{
    int player_id = this->getNextPlayerId();
    this->setNextPlayerId(++player_id);
    joinedPlayer->setID(player_id);
}

