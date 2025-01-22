// GUI.hpp
#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>

class GUI {
public:
    GUI(sf::RenderWindow& window); 

    // void handleEvents(const sf::Event& event, Network& network, bool& connected);
    void drawIntroScreen();
    void drawGameScreen();

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title;
    sf::Text inputText;
    sf::Text serverText;
    std::string inputBuffer;

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
