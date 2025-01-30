#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <ctime> 
#include <cstdlib> 

using namespace std;

class Room {
    public:
        int id;
        int turnId = -1;
        vector<int> players;
        char word[9] = "abstrakt";

        void create_room(vector<string>& words);
        void remove_player_by_fd(int fd);
};

#endif