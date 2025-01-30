#include "include/ServerManager.hpp"

void ServerManager::init_server() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(1);
    }

    struct sockaddr_in address;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        exit(1);
    }

    poll_fds.push_back({server_fd, POLLIN, 0});
}

void ServerManager::start() {
    init_server();

    while (true) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            perror("Poll error");
            break;
        }

        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_fd) {
                    accept_new_client();
                } else {
                    process_client_input(i);
                }
            }
        }
    }

    close(server_fd);
}

void ServerManager::accept_new_client() {
    int new_socket = accept(server_fd, nullptr, nullptr);
    if (new_socket < 0) {
        perror("Accept");
        return;
    }
    
    poll_fds.push_back({new_socket, POLLIN, 0});
    players[new_socket] = {new_socket, -1, ""};

    cout << "Accepted new client." << endl;
}

void ServerManager::process_client_input(int index) {
    char buffer[BUFFER_SIZE] = {0};
    int valread = read(poll_fds[index].fd, buffer, BUFFER_SIZE);

    if (valread <= 0) {
        remove_player(poll_fds[index].fd);
    } else {
        handle_client_input(poll_fds[index].fd, string(buffer, valread - 1));
    }
}

void ServerManager::remove_player(int fd) {
    if (players.find(fd) != players.end()) {
        leave_room(fd);
        players.erase(fd);
    }
    
    close(fd);
    poll_fds.erase(remove_if(poll_fds.begin(), poll_fds.end(), [fd](const pollfd& p) { return p.fd == fd; }),poll_fds.end());
    cout << "Removed a client." << endl;
}


void ServerManager::send_to_client(int fd, const string& message) {
    uint32_t length = message.size();

    send(fd, &length, sizeof(length), 0); // Najpierw wysyłamy długość wiadomości
    send(fd, message.c_str(), length, 0); // Następnie wysyłamy wiadomość
}

bool invalid_name(const std::string& str) {
    if (str.length() > 16) {
        return true;
    }
    if (str.find(';') != std::string::npos) {
        return true;
    }
    if (str.find(':') != std::string::npos) {
        return true;
    }
    if (str.find(',') != std::string::npos) {
        return true;
    }

    return false;
}

void ServerManager::handle_client_input(int fd, const string& input) {
    cout << input << endl;

    if (input.rfind("SET_NICKNAME", 0) == 0) {
        string nickname = input.substr(13);

        bool taken = any_of(players.begin(), players.end(), [&](const auto& pair) {
            return pair.second.nickname == nickname;
        });

        if (taken || invalid_name(nickname)) {
            send_to_client(fd, "WRONG_NICKNAME");
            return;
        }

        players[fd].nickname = nickname;
        send_to_client(fd, "SET_NICKNAME;" + nickname);
    } else if (input == "CREATE_ROOM") {
        if (players[fd].room_id > -1) return;
        Room room;
        room.id = next_room_id;
        rooms[next_room_id] = room;
        room.create_room(words);

        cout << "Room #" << next_room_id << " created!" << endl;
        
        join_to_room(fd, next_room_id);
        send_to_client(fd, "JOIN_ROOM;" + to_string(next_room_id));
        
        next_room_id++;
    } else if (input == "ROOM_LIST") {
        string roomsList = "ROOM_LIST";

        for (const auto& pair : rooms) {
            const Room& room = pair.second;
            roomsList += ";" + std::to_string(room.id) + "," + std::to_string(room.players.size());
        }

        send_to_client(fd, roomsList);
    } else if (input.rfind("JOIN_ROOM", 0) == 0) {
        int roomId = std::stoi(input.substr(input.find(';') + 1));

        join_to_room(fd, roomId);
    } else if (input == "LEAVE_ROOM") {
        leave_room(fd);
        send_to_client(fd, "LEAVE_ROOM");
        handle_client_input(fd, "ROOM_LIST");
    } else if (input  == "START_ROOM") {
        start_game(fd);
    } else if (input.rfind("GUESS", 0) == 0) {
        string character = input.substr(input.find(';') + 1);
        guess(fd, character);
    }
}

void ServerManager::join_to_room(int fd, int roomId) {
    if (rooms.find(roomId) == rooms.end()) return;
    if (players.find(fd) == players.end()) return;
    
    Room& room = rooms[roomId];

    if (room.players.size() >= MAX_PLAYERS) {
        return send_to_client(fd, "ERROR;TOO_MANY_PLAYERS");
    }

    Player& player = players[fd];
    if (player.room_id > -1) return;

    player.room_id = roomId;
    room.players.push_back(fd);

    send_to_client(fd, "JOIN_ROOM;" + to_string(roomId));
    update_room_players(roomId);

    cout << "Player " << fd << " joined to room " << roomId << endl;
}

void ServerManager::leave_room(int fd) {
    Player& player = players[fd];
    int room = player.room_id;

    if (room > -1 && rooms.find(room) != rooms.end()) {
        rooms[room].remove_player_by_fd(fd);

        if (rooms[room].players.size() <= 0) {
            rooms.erase(room);
        } else {
            update_room_players(room);
        }

        player.room_id = -1;
        for (int i = 0; i < 8; i++) player.guessed[i] = 0;
        player.missed = 0;
    } else if (room > -1) {
        player.room_id = -1;
    }

}

void ServerManager::update_room_players(int roomId) {
    Room& room = rooms[roomId];
    if (rooms.count(roomId) == 0) return;

    string playersData = "";
    for (int i = 0; i < room.players.size(); i++) {
        if (players.count(room.players[i]) == 0) continue;
        playersData += players[room.players[i]].nickname + ",";
        for (const auto& guess : players[room.players[i]].guessed) playersData += to_string(guess);
        playersData += "," + to_string(players[room.players[i]].missed);
        playersData += ":";
    }

    string data = playersData;
    cout << "player data: " << data << endl;
    for (int i = 0; i < room.players.size(); i++) {
        send_to_client(room.players[i], "UPDATE_ROOM;" + data + ";" + to_string(i) + ";" + to_string(room.turnId));
    }
}

void ServerManager::start_game(int fd) {
    if (players.find(fd) == players.end()) return;

    Player player = players[fd];
    int room_id = player.room_id;
    if (rooms.find(room_id) == rooms.end()) return;

    Room& room = rooms[room_id];
    if (room.players[0] != fd) return; // nie owner
    if (room.turnId > -1) return; //juz zaczete
    if (room.players.size() < 2) return; //mniej niz 2 graczy


    room.turnId = 0;

    update_room_players(room_id);
}

void ServerManager::next_turn(int room_id) {
    if (rooms.find(room_id) == rooms.end()) return;
    Room& room = rooms[room_id];

    int alive = 0;
    for (int i = 0; i < room.players.size(); i++) {
        int fd = room.players[i];
        if (players.find(fd) == players.end()) continue;
        Player player = players[fd];
        
        if (player.missed < 6 && player.hits < 8) alive++;
    }

    if (alive < 2) return end_game(room_id);

    if (room.turnId+1 >= room.players.size()) room.turnId = 0;
    else room.turnId++;

    if (players[room.players[room.turnId]].missed >= 6) {
        players[room.players[room.turnId]].missed++;
        return next_turn(room_id);
    } else if (players[room.players[room.turnId]].hits >= 8) {
        return next_turn(room_id);
    }

    update_room_players(room_id);
} 

void ServerManager::end_game(int room_id) {
    if (rooms.find(room_id) == rooms.end()) return;
    Room& room = rooms[room_id];

    vector<Player> roomPlayers;
    for (int i = 0; i < room.players.size(); i++) {
        int fd = room.players[i];
        if (players.find(fd) == players.end()) continue;
        Player player = players[fd];
        roomPlayers.push_back(player);
    }

    std::sort(roomPlayers.begin(), roomPlayers.end(), [](const Player& a, const Player& b) {
        return a.missed < b.missed;
    });
   
    string usernames = "";
    for (int i =0 ; i< 3; i++) {
        usernames += roomPlayers[i].nickname + ":";
    }

    
    for (const auto& player : roomPlayers) {
        cout << "END GAME " << endl;
        send_to_client(player.fd, "END_GAME;"+usernames);
    }
    
    rooms.erase(room_id);
}

void ServerManager::guess(int fd, string& character) {
    if (players.find(fd) == players.end()) return;
    if (character.empty()) return;

    Player& player = players[fd];
    int room_id = player.room_id;
    if (rooms.find(room_id) == rooms.end()) return;
    Room& room = rooms[room_id];

    if (room.players[room.turnId] != fd) return; //nie twoja tura

    cout << player.nickname << " guessed " << character << endl;

    int hits = 0;
    for (int i = 0; i < 8; i++) {
        char c = room.word[i];
        if (c == character[0] && player.guessed[i] == 0) {
            player.guessed[i] = 1;
            hits++;
            send_to_client(fd, "GUESSED;"+character);
        }
    }

    if (hits == 0) player.missed++;
    else player.missed += hits;


    next_turn(room_id);
}