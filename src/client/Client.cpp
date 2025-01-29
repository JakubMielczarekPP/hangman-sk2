#include "include/Client.hpp"

bool Client::connect_to_server(string& serverIp, int port) {
    if (socket.connect(serverIp, port) == sf::Socket::Done) {
        connected = true;

        //lista pokoi
        string cmd = "ROOM_LIST";
        send_to_server(cmd);

        return true;
    }

    return false;
}

void Client::send_to_server(string& message) {
    if (connected) socket.send(message.c_str(), message.size()+1);
}

void Client::receive_message() {
    char data[1024];
    std::size_t received;
    sf::Socket::Status status = socket.receive(data, sizeof(data), received);

    if (status == sf::Socket::Done) {
        std::string message(data, received); 
        if (message.empty()) return;
        
        if (message.find("ROOM_LIST") == 0) {
            process_room_list(message);
            return;
        } else if (message.find("JOIN_ROOM") == 0) {
            roomId = std::stoi(message.substr(message.find(';') + 1));
            return;
        } else if (message == "LEAVE_ROOM") {
            roomId = -1;
            return;
        } else if (message == "WRONG_NICKNAME") {
            error = "Nickname must be unique, less than 18 characters, can't contain \";\" and \":\"!";
            return;
        } else if (message.find("SET_NICKNAME") == 0) {
            nickname = message.substr(message.find(';') + 1);
            return;
        }

    } 
}

void Client::process_room_list(string& message) {
    rooms.clear();

    std::cout<<message<<std::endl;
    std::stringstream ss(message);
    std::string token;

    std::getline(ss, token, ';');

    while (std::getline(ss, token, ';')) {
        std::stringstream roomData(token);
        std::string roomIdStr, playersCountStr;

        std::getline(roomData, roomIdStr, ',');
        std::getline(roomData, playersCountStr, ',');

        Room room;
        room.roomId = std::stoi(roomIdStr);
        room.playersCount = std::stoi(playersCountStr); 

        rooms.push_back(room);
    }

    std::cout << "Rooms:" << std::endl;
    for (const Room& room : rooms) {
        std::cout << "Room ID: " << room.roomId << ", Players: " << room.playersCount << std::endl;
    }
}