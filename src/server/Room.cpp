#include "include/Room.hpp"

void Room::create_room(vector<string>& words) {
    if (words.empty()) return; 

    int randomIndex = rand() % words.size(); 
    std::string chosenWord = words[randomIndex];

    std::strncpy(word, chosenWord.c_str(), 8);
    word[8] = '\0';

    std::cout << "Chosen word: " << word << std::endl;
}

void Room::remove_player_by_fd(int fd) {
    auto it = std::remove(players.begin(), players.end(), fd);
    players.erase(it, players.end());
}