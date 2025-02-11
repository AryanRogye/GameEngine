#include "client/client.h"
#include "packet.h"
#include <mutex>

/** Deconstructor For The Client Class **/
Client::~Client() 
{
    enet_peer_disconnect(this->peer, 0);
    enet_host_flush(this->client);
    enet_peer_reset(this->peer);
    enet_host_destroy(this->client);
    enet_deinitialize();
}

/** Get The Player Object **/
Player* Client::getPlayer() { return this->player; }

/** Client Constructor Main Logic For Client In Here **/
Client::Client() 
{
    // Initialize The Player Object
    this->player = new Player(0, "Player 1");
    this->players = std::vector<Player*>();
    // Create Socket File Descriptor
    
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        exit(EXIT_FAILURE);
    }
    this->client = enet_host_create(
        NULL,       // create a client host
        1,          // only allow 1 outgoing connection
        2,          // allow up 2 channels to be used, 0 and 1
        0,          // assume any amount of incoming bandwidth
        0           // assume any amount of outgoing bandwidth
    );
    if (!this->client)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }
    // Connect to the server
    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = PORT;

    this->peer = enet_host_connect(
        this->client,       // client host
        &address,           // address of the server
        2,                  // number of channels
        0                   // data to associate with the peer
    );
    if (!this->peer)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    if (enet_host_service(this->client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) 
    {
        std::cout << "Connected to server!" << std::endl;
    }
    else 
    {
        std::cerr << "Failed to connect to server." << std::endl;
        enet_peer_reset(this->peer);
        exit(EXIT_FAILURE);
    }
    // Listen for incoming Messages From The Server in a new thread
    std::thread t(&Client::listenToServer, this);
    t.detach();

    // Add A Listener for the players inside the client
}

/** 
 * This Function is For Listening To The Server
 * This Function Will Run In A Seperate Thread
 **/
void Client::listenToServer()
{
    
    ENetEvent event;
    while (true)
    {
        while(enet_host_service(this->client, &event, 1000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    this->handleRecievedPacket(event.packet->data, event.packet->dataLength);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Disconnected from server.\n";
                    return;
                default:
                    break;
            }
        }
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
        /** 
         * This is Used For The New Player
         * To Get Information About The Existing Players
         **/
        case PacketType::SEND_EXISTING_CLIENTS_NEW_PLAYER:
            std::cout << "Just Recieved Data About Other Clients" << std::endl;
            std::cout << "processing...." << std::endl;
            this->handleExistingPlayers(buffer, bytesRecieved, &offset);
            break;
    
        /** 
         * This Is When A New Player Has Joined The Game 
         * This is really used for the existing players to know
         * when a new player has joined the game
         **/

        case PacketType::SEND_NEW_PLAYER_EXISTING_CLIENTS:
            std::cout << "A New Player Has Joined The Game" << std::endl;
            std::cout << "Need To Add Him To The Screen.. Wait" << std::endl;
            handleNewPlayerJoined(buffer, bytesRecieved, &offset);
            break;
        case PacketType::PLAYER_JOINED:
            std::cout << "!!!!!!!!!!!THIS SHOULD NOT GET CALLED BADDDDDD" << std::endl;
            break;
        case PacketType::PLAYER_MOVED:
            std::cout << "Some Other Player Moved" << std::endl;
            this->handleOtherPlayersMoved(buffer, bytesRecieved, &offset);
            break;
        case PacketType::ASSIGN_PLAYER_ID:
            std::cout << "Got Player ID From Server" << std::endl;
            handleIDRecieved(buffer, bytesRecieved, &offset);
            break;
        case PacketType::SEND_PLAYER_SPRITE_INDEX:
            handleSpriteRecieved(buffer, bytesRecieved, &offset);
            std::cout << "Got Player Texture From Server" << std::endl;
            break;
        default:
            std::cout << "Unknown Packet Type" << std::endl;
            break;
    }
}

void Client::handleSpriteRecieved(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset)
{
    SendPlayerSpriteIndex idx = SendPlayerSpriteIndex();
    idx.deserialize(buffer, offset);

    // We Dont Wanna do Anything with our ID
    if (idx.getID() == this->player->getID()) return;
    
    std::lock_guard<std::mutex> lock(this->playersMutex);
    for (Player* p : this->players)
    {
        if (p->getID() == idx.getID())
        {
            std::cout << "Got Player [" << idx.getID() <<"] Texture Index: " << idx.getSpriteIndex() << std::endl;
            p->setSpriteIndex(idx.getSpriteIndex());
            return;
        }
    }
}

void Client::handleExistingPlayers(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset)
{
    // Now That We Have The Data We Want to deserialize it
    SendExistingClientsToNewPlayer existingClients = SendExistingClientsToNewPlayer();
    existingClients.deserialize(buffer, offset);
    // Loop Through The Players And Add Them To The List
    for (Player player : existingClients.getPlayers())
    {
        Player* newPlayer = new Player(player.getID(), player.getName());
        std::cout << "Got New Player: " << player.getName() << std::endl;
        std::cout << "Got New Player ID: " << player.getID() << std::endl;
        {
            // Lock The Vector For Safe Modification
            std::lock_guard<std::mutex> lock(this->playersMutex);
            this->players.push_back(newPlayer);
        }
    }
}

/** 
 * This Function is For Handling When Other Players Move
 **/
void Client::handleOtherPlayersMoved(uint8_t* buffer, ssize_t bytesRecieved, size_t* offset)
{
    PlayerMoved playerMoved = PlayerMoved();
    playerMoved.deserialize(buffer, offset);

    // Lock The Players Vector While Looping
    std::lock_guard<std::mutex> lock(this->playersMutex);
    for (Player* p : this->players)
    {
        if (p->getID() == playerMoved.getID())
        {
            // Determine facing direction based on x-coordinate change
            int oldX = p->getX();
            int newX = playerMoved.getX();
            
            if (newX > oldX) {
                p->setFacingRight(true);
            }
            else if (newX < oldX) {
                p->setFacingRight(false);
            }
            // Update the player's position
            p->setX(newX);
            p->setY(playerMoved.getY());
            
            return;
        }
    }
}

void Client::handleNewPlayerJoined(uint8_t* buffer,ssize_t bytesRecieved,size_t* offset)
{
    PlayerJoined playerJoined = PlayerJoined();
    playerJoined.deserialize(buffer, offset);

    // Add The New Player To The List Of Players
    Player* newPlayer = new Player(playerJoined.getID(), playerJoined.getName().c_str());

    {
        // Lock The Vector for safe modification
        std::lock_guard<std::mutex> lock(this->playersMutex);
        this->players.push_back(newPlayer);
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

bool Client::handlePlayerJoined(std::string name)
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
    return true;
}

const std::vector<Player*>& Client::getPlayers()
{
    std::lock_guard<std::mutex> lock(this->playersMutex);
    return this->players;
}
std::vector<Player*> Client::getPlayersSafe() 
{
    std::lock_guard<std::mutex> lock(this->playersMutex);
    return this->players; // returns a copy, safe for iteration
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

    ENetPacket* packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    if (!packet)
    {
        return false;
    }

    enet_peer_send(this->peer, 0, packet);
    enet_host_flush(this->client);

    return true;
}

bool Client::handleSendingPlayerTexture(int spriteIndex)
{
    SendPlayerSpriteIndex idx = SendPlayerSpriteIndex(this->player->getID(), spriteIndex);
    uint8_t buffer[1024];
    size_t offset = idx.serialize(buffer);
    if (!sendMessageToServer(offset, buffer))
    {
        std::cout << "There Was An Error Sending The Player Texture To The Server" << std::endl;
        return false;
    }
    return true;
}


/** Have To Create a packet for this **/
void Client::handleZombieSpawn(int maxRow, int maxCol)
{

}
