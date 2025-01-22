#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <algorithm>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024

using namespace std;

struct Player {
    int fd;
    int roomId = -1;
    string nickname;
};

struct Room {
    int id;
    set<int> players;
    bool inProgress = false;
};

vector<struct pollfd> poll_fds;
map<int, Player> players;
map<int, Room> rooms;
int nextRoomId = 1;

// Usuwanie gracza
void remove_player(int fd) {
    if (players.find(fd) != players.end()) {
        int room = players[fd].roomId;
        if (room > -1 && rooms.find(room) != rooms.end()) {
            rooms[room].players.erase(fd);
            if (rooms[room].players.empty()) {
                rooms.erase(room);
            }
        }
        players.erase(fd);
    }
    close(fd);
    poll_fds.erase(remove_if(poll_fds.begin(), poll_fds.end(),
                             [fd](const pollfd& p) { return p.fd == fd; }),
                   poll_fds.end());
}

// Wysyłanie wiadomości do klienta
void send_to_client(int fd, const string& message) {
    send(fd, message.c_str(), message.size(), 0);
}

// Obsługa danych od klienta
void handle_client_input(int fd, const string& input) {
    auto& player = players[fd];
    if (player.nickname.empty()) {
        player.nickname = input;
        send_to_client(fd, "Welcome " + player.nickname + "!\n");
        send_to_client(fd, "Available commands:\n1. LIST_ROOMS\n2. CREATE_ROOM\n3. JOIN_ROOM <id>\n");
    } else if (input == "LIST_ROOMS") {
        string response = "Available rooms:\n";
        for (const auto& [id, room] : rooms) {
            response += "Room " + to_string(id) + " (" + to_string(room.players.size()) + " players)\n";
        }
        send_to_client(fd, response);
    } else if (input == "CREATE_ROOM") {
        Room newRoom = {nextRoomId++, {fd}, false};
        rooms[newRoom.id] = newRoom;
        player.roomId = newRoom.id;
        send_to_client(fd, "Room " + to_string(newRoom.id) + " created! Waiting for players...\n");
    } else if (input.rfind("JOIN_ROOM", 0) == 0) {
        int roomId = stoi(input.substr(10));
        if (rooms.find(roomId) != rooms.end()) {
            rooms[roomId].players.insert(fd);
            player.roomId = roomId;
            send_to_client(fd, "Joined Room " + to_string(roomId) + ". Waiting for game to start...\n");
        } else {
            send_to_client(fd, "Room " + to_string(roomId) + " does not exist.\n");
        }
    } else {
        send_to_client(fd, "Unknown command.\n");
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
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
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        exit(1);
    }

    poll_fds.push_back({server_fd, POLLIN, 0});

    while (true) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            perror("Poll error");
            break;
        }

        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_fd) {
                    int new_socket = accept(server_fd, nullptr, nullptr);
                    if (new_socket < 0) {
                        perror("Accept");
                        continue;
                    }
                    poll_fds.push_back({new_socket, POLLIN, 0});
                    players[new_socket] = {new_socket, -1, ""};
                    send_to_client(new_socket, "Enter your nickname:\n");
                } else {
                    char buffer[BUFFER_SIZE] = {0};
                    int valread = read(poll_fds[i].fd, buffer, BUFFER_SIZE);
                    if (valread <= 0) {
                        remove_player(poll_fds[i].fd);
                    } else {
                        handle_client_input(poll_fds[i].fd, string(buffer, valread - 1));
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
