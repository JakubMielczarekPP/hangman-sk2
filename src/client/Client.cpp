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
    std::size_t received = 0; 

    if (socket.receive(&length, sizeof(length), received) != sf::Socket::Done) {
        return;
    }

    char data[1024];

    if (length > sizeof(data)) {
        return;
    }

    if (socket.receive(data, length, received) != sf::Socket::Done) {
        return;
    }

    std::string message(data, received); 
    if (message.empty()) return;
    cout << "message: " << message << endl;
    if (message.find("ROOM_LIST") == 0) {
        process_room_list(message);
        return;
    } else if (message.find("JOIN_ROOM") == 0) {
        roomId = std::stoi(message.substr(message.find(';') + 1));
        roomData = {roomId, -1, -2, false, {}};
        std::fill(std::begin(userGuessed), std::end(userGuessed), '\0');
        return;
    } else if (message == "LEAVE_ROOM") {
        roomId = -1;
        winners.clear();
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
    } else if (message.find("END_GAME") == 0) {
        string winner = message.substr(message.find(';') + 1);
        
        cout << winner << endl;
        std::stringstream ss(winner);
        std::string line;
        
        vector<string> myWinners;
        while (std::getline(ss, line, ':')) {
            cout << line << endl;
            myWinners.push_back(line);
        }

        winners = myWinners;
    } else if (message.find("GUESSED") == 0) {
        std::stringstream ss(message.substr(message.find(';') + 1));
        std::string character, indexStr;
        std::getline(ss, character, ';');
        std::getline(ss, indexStr, ';');
        int index = std::stoi(indexStr);

        userGuessed[index] = character[0];
        cout << "Guessed character: " << character << " at index: " << index << endl;
    }
}

void Client::process_room_data(string& data) {
    vector<PlayerData> players;
    std::stringstream ss(data);
    std::string line;
    
    int k = 0, turnId = -1, playerPosition = -2;
    bool isOwner = false;
    while (std::getline(ss, line, ';')) {
        std::stringstream ss3(line);
        std::string user;
        cout << line << endl;
        if (k == 0) {
            while (std::getline(ss3, user, ':')) {
                std::stringstream ss2(user);
                std::string element;

                int index = 0, missed = 0;
                string nickname;
                char guessed[8];
                while (std::getline(ss2, element, ',')) {
                    if (index == 0) nickname = element;
                    else if (index == 1) {
                        for (int i = 0; i < 8; i++) {
                            guessed[i] = element[i];
                        }
                    } else {
                        missed = stoi(element);
                    }
                    index++;
                }

                players.push_back({nickname, {}, missed});
                std::copy(std::begin(guessed), std::end(guessed), std::begin(players.back().guessed));
            }
        } else if (k==1) {
            if (line == "0") isOwner = true;
            playerPosition = stoi(line);
        } else {
            turnId = stoi(line);
        }

        k++;

    }

    roomData = {roomId, turnId, playerPosition, isOwner, players};
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