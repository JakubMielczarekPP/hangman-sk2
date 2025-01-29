#include "include/Room.hpp"

void Room::create_room() {

}

void Room::remove_player_by_fd(int fd) {
    auto it = std::remove(players.begin(), players.end(), fd);
    players.erase(it, players.end());
}