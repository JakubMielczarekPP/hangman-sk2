// GUI.hpp
#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <vector>

#include "Client.hpp"

struct JoinButton {
    sf::RectangleShape button;
    int id;
};

class GUI {
public:
    GUI(sf::RenderWindow& window);

    // void handleEvents(const sf::Event& event, Network& network, bool& connected);
    void drawIntroScreen(Client& client);
    void drawGameScreen(Client& client);
    void drawLobbyScreen(Client& client);
    void drawResults(Client& client);
    void handle_input(sf::Event event, Client& client);
    void handle_clicks(Client& client);
    void handle_scroll(sf::Event event);

    vector<JoinButton> joinButtons;

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text inputText;
    sf::Text serverText;

    int scrollOffset = 0;

    std::string inputBuffer;
    sf::Text title;
    sf::Text tip;

    sf::RectangleShape createRoomButton;
    sf::Text createRoomText;

    sf::RectangleShape refreshButton;
    sf::Text refreshText;

    sf::RectangleShape leaveRoomButton;
    sf::Text leaveRoomText;

    sf::RectangleShape startRoomButton;
    sf::Text startRoomText;

    void updateInputText();
};

#endif
