#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class Client {
public:
    bool connected;

    bool connect_to_server(string& serverIp, int port);
    void send_message(string& message);
    string receive_message();

private:
    sf::TcpSocket socket;
};

#endif
