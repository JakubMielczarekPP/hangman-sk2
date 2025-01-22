#include "include/GUI.hpp"

GUI::GUI(sf::RenderWindow& window) : window(window) {
    font.loadFromFile("assets/arial.ttf");
    title.setFont(font);
    inputText.setFont(font);
    serverText.setFont(font);
    title.setString("Enter Server IP:");
    title.setPosition(20, 20);

    drawIntroScreen();
}

void GUI::drawIntroScreen() {
    window.draw(title);
    window.draw(inputText);
}

void GUI::drawGameScreen() {
    window.draw(serverText);
    window.draw(createRoomButton);
    window.draw(createRoomText);
}

void GUI::updateInputText() {
    inputText.setString("Input: " + inputBuffer);
}
