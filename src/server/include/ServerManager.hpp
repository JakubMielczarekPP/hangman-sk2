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

#define MAX_PLAYERS 4
#define BUFFER_SIZE 1024

using namespace std;

struct Player {
    int fd;
    int room_id = -1;
    string nickname;
    int guessed[8] = {0,0,0,0,0,0,0,0};
    int missed = 0;
    int hits = 0;
};

class ServerManager {
    public:
        int port;
        void start();
        
        vector<string> words = {
            "samoloty", "piosenka", "kartonik", "informat", 
            "mieszkan", "chomikuj", "teleport", "wazoniki",
            "zabawnik", "ogrodnik", "przystan", "wyscigow", "mechanik",
            "ogrodowa", "przyklad", "zapiekli", "dodatnie"
        };
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
        void join_to_room(int fd, int room);
        void leave_room(int fd);
        void update_room_players(int room);
        void start_game(int fd);
        void guess(int fd, string& character);
        void next_turn(int room_id);
        void end_game(int room_id);
};

#endif