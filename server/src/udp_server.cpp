#include "udp_server.h"
#include "packet.h"
#include <cstdint>

UDPServer::~UDPServer()
{
    enet_host_destroy(server);
    enet_deinitialize();
}
void UDPServer::handleCommands() {
    std::string input;
    bool running = true;
    while (running) {
        std::getline(std::cin, input);  // This call blocks until the user enters a line.
        if (input == "list") {
            printPlayers();
        }
        else if (input == "quit" || input == "exit") {
            std::cout << "Shutting down the server." << std::endl;
            running = false;
            // You might also want to notify the main thread to clean up immediately.
        }
        else {
            std::cout << "Unknown command: " << input << std::endl;
        }
    }
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

    // Start the command handler in a separate thread.
    std::thread commandThread(&UDPServer::handleCommands, this);
    commandThread.detach();

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
                    /*std::cout << "Received packet of size: " << event.packet->dataLength << std::endl;*/
                    handlePacket(event.packet, event.peer);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected.\n";
                    // We Want to send the other players that the person disconnected but im gonna do sprites first
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
        case PacketType::SEND_PLAYER_SPRITE_INDEX:
            handlePlayerSpriteIndex(packet, peer, &offset);
            break;
        case PacketType::SEND_MAP_DATA:
            if (!this->receivedMapData)
            {
                handleMapData(packet, peer, &offset);
                this->receivedMapData = true;
            }
            break;
        default:
            std::cout << "Unknown Packet Type" << std::endl;
            break;
    }
}

void UDPServer::handleMapData(ENetPacket* packet, ENetPeer* peer, size_t* offset)
{
    SendMapData mapData = SendMapData();
    mapData.deserialize(packet->data, offset);
        
    std::vector<std::vector<int>> map = mapData.getMapData();
    std::cout << "Received map data from client: " << std::endl;
    // Determine max width for proper spacing
    int maxWidth = 0;
    for (const auto& row : map) {
        for (int num : row) {
            int width = std::to_string(num).length();
            if (width > maxWidth) {
                maxWidth = width;
            }
        }
    }
    for (const auto& row : map) {
        for (int num : row) {
            std::cout << std::setw(maxWidth + 1) << num << " ";
        }
        std::cout << std::endl;
    }

    // In The Map 2D is a door for a house that we can assign to a villager
}

void UDPServer::handlePlayerSpriteIndex(ENetPacket* packet,ENetPeer* peer, size_t* offset)
{
    SendPlayerSpriteIndex idx = SendPlayerSpriteIndex();
    idx.deserialize(packet->data, offset);

    uint8_t buffer[1028];
    *offset = idx.serialize(buffer);

    // Sending to all the other clients
    for (auto client: clientPeers)
    {
        // Make Sure Not To Send to the same client
        if (client.second == peer) continue;
        if (!this->sendMessageToClient(*offset, buffer, client.second))
        {
            std::cout << "Couldlnt Send Sprite Info To Other Players" << std::endl;
            return;
        }
    }
    // Want to add to the players vector
    for (auto& player : this->players)
    {
        if (player.getID() == idx.getID())
        {
            player.setSpriteIndex(idx.getSpriteIndex());
            break;
        }
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

    // We Want to check if the player is inside our game
    // Need to read the buffer and offset to get the values of the player
    // Generate A Unique ID for the player
    generateUnqiuePlayerId(&joinedPlayer);
    // Add the player addresses to the map

    // We Want to see if anyone else in our game has the same id

    bool playerExists = false;
    // Check if the player ID is already in the game
    auto it = clientPeers.find(joinedPlayer.getID());
    if (it != clientPeers.end()) 
    {
        std::cout << "Player Is Rejoining: ID = " << joinedPlayer.getID() << std::endl;
        // Update their peer in case they disconnected before
        it->second = peer; // Update the peer to the new connection
        playerExists = true;
    }

    // If the player does not exist, add them
    if (!playerExists) 
    {
        // Assign unique ID if needed
        generateUnqiuePlayerId(&joinedPlayer);

        // Store the player's peer
        this->clientPeers[joinedPlayer.getID()] = peer;

        // Add the player to the list
        Player player_to_add = Player(joinedPlayer.getID(), joinedPlayer.getName().c_str());
        this->players.push_back(player_to_add);

        std::cout << "New Player Joined: " << joinedPlayer.getName() << " ID: " << joinedPlayer.getID() << std::endl;
    }


    sendPlayerID(joinedPlayer.getID(), peer);       /** TODO Need to Fix This **/
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

    // We Also Wanna Send The Sprite Index To The New Player
    for (auto player : this->players)
    {
        SendPlayerSpriteIndex idx = SendPlayerSpriteIndex(player.getID(), player.getSpriteIndex());
        uint8_t buffer[1024];
        size_t offset = idx.serialize(buffer);
        if (!sendMessageToClient(offset, buffer, peer))
        {
            std::cout << "Failed to send sprite index to new player" << std::endl;
            return false;
        }
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


void UDPServer::printPlayers()
{
    std::cout << "Players in the game: " << std::endl;
    for (Player player : this->players)
    {
        std::cout << "ID: " << player.getID() << " Name: " << player.getName() << " Sprite Index: " << player.getSpriteIndex() << std::endl;
    }
}
