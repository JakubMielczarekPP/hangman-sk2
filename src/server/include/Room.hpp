#ifndef ROOM_HPP
#define ROOM_HPP

#include <set>

using namespace std;

class Room {
    public:
        int id;
        set<int> players;
        bool in_progress = false;

        void create_room();
};

#endif