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
        int room = players[fd].room_id;
        if (room > -1 && rooms.find(room) != rooms.end()) {
            rooms[room].players.erase(fd);
            if (rooms[room].players.empty()) {
                rooms.erase(room);
            }
        }
        players.erase(fd);
    }
    
    close(fd);
    poll_fds.erase(remove_if(poll_fds.begin(), poll_fds.end(), [fd](const pollfd& p) { return p.fd == fd; }),poll_fds.end());
    cout << "Removed a client." << endl;
}


void ServerManager::send_to_client(int fd, const string& message) {
    send(fd, message.c_str(), message.size(), 0);
}

void ServerManager::handle_client_input(int fd, const string& input) {
    cout << input << endl;

    if (input.rfind("SET_NICKNAME", 0) == 0) {
        string nickname = input.substr(12);
        cout << nickname << endl;
    } else if (input == "CREATE_ROOM") {
        Room room;
        room.id = next_room_id;
        rooms[next_room_id] = room;
        room.create_room();

        cout << "Room #" << next_room_id << " created!" << endl;
        
        next_room_id++;
    } else if (input == "ROOM_LIST") {
        string roomsList = "ROOM_LIST";
        
        for (int i = 0; i < rooms.size(); i++) {
            Room room = rooms[i];
            roomsList += ";" + to_string(room.id) + "," + to_string(room.players.size());
        }

        send_to_client(fd, roomsList);
    }
}
