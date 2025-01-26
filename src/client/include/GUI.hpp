// GUI.hpp
#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>

#include "Client.hpp"

class GUI {
public:
    GUI(sf::RenderWindow& window);

    // void handleEvents(const sf::Event& event, Network& network, bool& connected);
    void drawIntroScreen();
    void drawGameScreen();
    void drawLobbyScreen();
    void handle_input(sf::Event event, Client& client);
    void handle_clicks(sf::Event event, Client& client);

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text inputText;
    sf::Text serverText;

    std::string inputBuffer;
    sf::Text title;
    sf::Text tip;

    // Przyciski
    sf::RectangleShape createRoomButton;
    sf::Text createRoomText;
    sf::RectangleShape listRoomsButton;
    sf::Text listRoomsText;
    sf::RectangleShape joinRoomButton;
    sf::Text joinRoomText;

    void updateInputText();
};

#endif
