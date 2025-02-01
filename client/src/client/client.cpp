#include "client/client.h"

/** Deconstructor For The Client Class **/
Client::~Client() 
{
    close(this->sockfd);

    // Lock to safely delete player objects
    std::lock_guard<std::mutex> lock(this->playersMutex);

    for (Player* player : this->players) {
        delete player;  // Free memory for each Player
    }
    this->players.clear();
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

    // Add A Listener for the players inside the client
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
        /** 
         * This is Used For The New Player
         * To Get Information About The Existing Players
         **/
        case PacketType::SEND_EXISTING_CLIENTS_NEW_PLAYER:
            std::cout << "Just Recieved Data About Other Clients" << std::endl;
            std::cout << "processing...." << std::endl;
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
        default:
            std::cout << "Unknown Packet Type" << std::endl;
            break;
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
            p->setX(playerMoved.getX());
            p->setY(playerMoved.getY());
            return;  // Found and updated the player; exit the function.
        }
    }
    std::cout << "Player with ID " << playerMoved.getID() << " not found." << std::endl;
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
    ssize_t bytesSent = sendto(
        this->sockfd,
        buffer,
        offset,
        0,
        (const struct sockaddr *) &this->serverAddress, sizeof(this->serverAddress)
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
