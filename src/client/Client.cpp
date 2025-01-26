#include "include/Client.hpp"

bool Client::connect_to_server(string& serverIp, int port) {
    if (socket.connect(serverIp, port) == sf::Socket::Done) {
        connected = true;
        scene = 1;
        return true;
    }

    return false;
}

void Client::send_to_server(string& message) {
    if (connected) socket.send(message.c_str(), message.size()+1);
}