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

class Client {
public:
    bool connected;
    int scene = 0;

    std::vector<Room> rooms;

    bool connect_to_server(string& serverIp, int port);
    void send_message(string& message);
    void send_to_server(string& message);
    
    void receive_message();

private:
    sf::TcpSocket socket;

    void process_room_list(string& message);
};

#endif
