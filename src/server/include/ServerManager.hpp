#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

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

#include "Room.hpp"

#define BUFFER_SIZE 1024

using namespace std;

struct Player {
    int fd;
    int room_id = -1;
    string nickname;
};

class ServerManager {
    public:
        int port;
        void start();
    
    private:
        int server_fd;
        vector<struct pollfd> poll_fds;
        map<int, Player> players;
        map<int, Room> rooms;
        int next_room_id = 1;

        void init_server();
        void remove_player(int fd);
        void send_to_client(int fd, const string& message);
        void handle_client_input(int fd, const string& input);
        void accept_new_client();
        void process_client_input(int index);
};

#endif