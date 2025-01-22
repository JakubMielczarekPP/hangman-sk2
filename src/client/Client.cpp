#include "include/Client.hpp"

bool Client::connect_to_server(string& serverIp, int port) {
    if (socket.connect(serverIp, port) == sf::Socket::Done) {
        connected = true;
        return true;
    }

    return false;
}