#include "include/GUI.hpp"

GUI::GUI(sf::RenderWindow& window) : window(window) {
    font.loadFromFile("assets/arial.ttf");
    title.setFont(font);
    inputText.setFont(font);

    serverText.setFont(font);
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

void GUI::drawLobbyScreen(Client& client) {
    // Ustalamy widok przewijania
    sf::View scrollView(window.getView()); // Używamy domyślnego widoku
    // scrollView.setCenter(0, 0);  // Center startowe
    scrollView.setSize(window.getSize().x, window.getSize().y);  // Rozmiar widoku = rozmiar okna
    window.setView(scrollView);  // Przypisujemy nowy widok

    title.setString("Lobby");
    title.setPosition(20, 20);

    createRoomButton.setSize(sf::Vector2f(200, 50));
    createRoomButton.setPosition(150, 50);
    createRoomButton.setFillColor(sf::Color::Green);

    createRoomText.setFont(font);
    createRoomText.setString("Create Room");
    createRoomText.setPosition(160, 60);

    refreshButton.setSize(sf::Vector2f(200, 50));
    refreshButton.setPosition(350, 50);
    refreshButton.setFillColor(sf::Color::Blue);

    refreshText.setFont(font);
    refreshText.setString("Refresh");
    refreshText.setPosition(360, 60);

    window.draw(title);
    window.draw(createRoomButton);
    window.draw(createRoomText);
    window.draw(refreshButton);
    window.draw(refreshText);

    const int roomHeight = 50;
    const int maxRoomsVisible = window.getSize().y / roomHeight; 

    if (client.rooms.size() > maxRoomsVisible) {
        scrollOffset = std::max(0, scrollOffset); 
    }

    for (int i = scrollOffset; i < client.rooms.size(); i++) {
        sf::Text roomText;
        roomText.setFont(font);
        roomText.setString("Room #" + std::to_string(client.rooms[i].roomId));
        roomText.setPosition(20, 130 + (i - scrollOffset) * roomHeight); 
        sf::Text playerCount;
        playerCount.setFont(font);
        playerCount.setString("Players: " + std::to_string(client.rooms[i].playersCount) + "/4");
        playerCount.setPosition(200, 130 + (i - scrollOffset) * roomHeight); 

        sf::RectangleShape joinButton;
        joinButton.setSize(sf::Vector2f(100, roomHeight-20));
        joinButton.setPosition(400, 140 + (i - scrollOffset) * roomHeight);
        joinButton.setFillColor(sf::Color::Green);

        window.draw(roomText);
        window.draw(playerCount);
        window.draw(joinButton);
    }
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
    } else if (refreshButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        string cmd = "ROOM_LIST";
        client.send_to_server(cmd);
    }
}

void GUI::handle_scroll(sf::Event event) {
    if (event.mouseWheelScroll.delta > 0) {
        if (scrollOffset > 0) {
            scrollOffset--; 
        }
    } else if (event.mouseWheelScroll.delta < 0) {
        scrollOffset++;
    }
}