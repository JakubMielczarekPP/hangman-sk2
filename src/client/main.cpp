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
        window.clear();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::TextEntered) gui.handle_input(event, client);
            else if (event.type == sf::Event::MouseButtonPressed) gui.handle_clicks(event, client);
        }

        if (client.scene == 0) gui.drawIntroScreen();
        else gui.drawLobbyScreen();

        window.display();
    }

    return 0;
}
