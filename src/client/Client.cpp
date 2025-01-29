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
    uint32_t length;
    std::size_t received = 0;  // Deklaracja zmiennej received

    // Pierwsze wywołanie receive w celu odebrania długości wiadomości
    if (socket.receive(&length, sizeof(length), received) != sf::Socket::Done) {
        return;
    }

    char data[1024];

    // Jeśli długość wiadomości jest większa niż rozmiar bufora, można obsłużyć to w odpowiedni sposób
    if (length > sizeof(data)) {
        // Obsługa zbyt dużych wiadomości
        return;
    }

    // Drugie wywołanie receive w celu odebrania samej wiadomości
    if (socket.receive(data, length, received) != sf::Socket::Done) {
        return;
    }

    // Przekształcenie odebranych danych w string
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
        error = "Nickname must be unique, less than 18 characters, mustn't contain \";\", \":\" and \",\"!";
        return;
    } else if (message.find("SET_NICKNAME") == 0) {
        nickname = message.substr(message.find(';') + 1);
        return;
    } else if (message.find("UPDATE_ROOM") == 0) {
        string roomData = message.substr(message.find(';') + 1);
        process_room_data(roomData);
    }
}

void Client::process_room_data(string& data) {
    vector<PlayerData> players;
    std::stringstream ss(data);
    std::string user;

    while (std::getline(ss, user, ':')) {
        std::stringstream ss2(user);
        std::string element;

        int index = 0;
        string nickname;
        char guessed[8];
        while (std::getline(ss2, element, ',')) {
            if (index == 0) nickname = element;
            else {
                for (int i = 0; i < 8; i++) {
                    guessed[i] = element[i];
                }
            }

            index++;
        }

        players.push_back({nickname, {}});
        std::copy(std::begin(guessed), std::end(guessed), std::begin(players.back().guessed));
    }

    roomData = {roomId, -1, players};
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