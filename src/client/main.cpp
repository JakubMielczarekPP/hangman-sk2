#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#include "include/Client.hpp"
#include "include/GUI.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hangman SK2");
    Client client;
    GUI gui(window);

    while (window.isOpen()) {
        
    }

    return 0;
}
