#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

#include "include/Client.hpp"
#include "include/GUI.hpp"

void receive_loop(Client& client) {
    while (true) {
        client.receive_message();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Hangman SK2");
    Client client;
    GUI gui(window);

    std::thread receiverThread(receive_loop, std::ref(client));

    while (window.isOpen()) {
        window.clear();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::TextEntered) gui.handle_input(event, client);
            else if (event.type == sf::Event::MouseButtonPressed) gui.handle_clicks(event, client);
            else if (event.type == sf::Event::MouseWheelScrolled) gui.handle_scroll(event);
        }

        if (!client.connected || client.nickname.empty()) gui.drawIntroScreen(client);
        else if(client.roomId > 0) gui.drawGameScreen(client);
        else if(client.winners.size()) gui.drawResults(client);
        else gui.drawLobbyScreen(client);

        window.display();
    }

    receiverThread.detach();
    return 0;
}
