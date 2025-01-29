#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sstream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

struct Room {
    int playersCount;
    int roomId;
};

struct PlayerData {
    int playerId;
    string nickname;
    int hitMatrix[8] = {0,0,0,0,0,0,0,0};
};

struct RoomData {
    int roomId;
    int turnId;
    vector<PlayerData> players;
};

class Client {
public:
    bool connected;
    string nickname = "";
    string error = "";

    int roomId = -1;

    std::vector<Room> rooms;
    RoomData roomData;

    bool connect_to_server(string& serverIp, int port);
    bool set_nickname(string& nickname);
    void send_message(string& message);
    void send_to_server(string& message);
    
    void receive_message();

private:
    sf::TcpSocket socket;

    void process_room_list(string& message);
};

#endif
