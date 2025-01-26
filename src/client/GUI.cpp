#include "include/GUI.hpp"

GUI::GUI(sf::RenderWindow& window) : window(window) {
    font.loadFromFile("assets/arial.ttf");
    title.setFont(font);
    inputText.setFont(font);

    serverText.setFont(font);
    title.setPosition(20, 20);
    inputText.setPosition(20, 60);
    tip.setFont(font);
    tip.setPosition(20,100);
}

void GUI::drawIntroScreen() {
    title.setString("Enter Server IP:");

    window.draw(title);
    window.draw(tip);
    window.draw(inputText);
}

void GUI::drawGameScreen() {
    serverText.setString("siema");
}

void GUI::drawLobbyScreen() {
    title.setString("Lobby");
    
    sf::RectangleShape createRoomButton(sf::Vector2f(200, 50));
    createRoomButton.setPosition(20 ,500);
    createRoomButton.setFillColor(sf::Color::Green);

    createRoomText.setFont(font);
    createRoomText.setString("Create Room");
    createRoomText.setPosition(30, 510);

    window.draw(title);
    window.draw(createRoomButton);
    window.draw(createRoomText);
}

void GUI::updateInputText() {
    inputText.setString("Input: " + inputBuffer);
}

void GUI::handle_input(sf::Event event, Client& client) {
    if (event.text.unicode == '\r') {
        title.setString("Wron2g ip");
        if (!client.connected) {
            if (!client.connect_to_server(inputBuffer, 8080)) tip.setString("Wrong ip");
        }

        inputBuffer.clear();
    } else if (event.text.unicode == '\b') {  // Backspace
        if (!inputBuffer.empty()) inputBuffer.pop_back();
    } else {
        inputBuffer += static_cast<char>(event.text.unicode);
    }

    updateInputText();
}

void GUI::handle_clicks(sf::Event event, Client& client) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    if (createRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        string cmd = "CREATE_ROOM";
        client.send_to_server(cmd);
    }
}