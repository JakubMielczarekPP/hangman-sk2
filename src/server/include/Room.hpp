#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <algorithm>

using namespace std;

class Room {
    public:
        int id;
        int turnId = -1;
        vector<int> players;
        bool in_progress = false;

        void create_room();
        void remove_player_by_fd(int fd);
};

#endif