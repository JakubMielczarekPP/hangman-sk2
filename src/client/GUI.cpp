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

void GUI::drawIntroScreen(Client& client) {
    if (!client.connected) title.setString("Enter Server IP:");
    else title.setString("Enter you nickname:");

    tip.setString(client.error);

    window.draw(title);
    window.draw(tip);
    window.draw(inputText);
}

void GUI::drawGameScreen(Client& client) {
    title.setString("Room #" + to_string(client.roomId));

    sf::RectangleShape yLine(sf::Vector2f(5, window.getSize().y));
    yLine.setPosition(window.getSize().x/2, 0);

    sf::RectangleShape xLine(sf::Vector2f(window.getSize().x, 5));
    xLine.setPosition(0, window.getSize().y/2);

    leaveRoomButton.setSize(sf::Vector2f(150, 30));
    leaveRoomButton.setPosition(window.getSize().x-150, window.getSize().y-30);
    leaveRoomButton.setFillColor(sf::Color::Red);

    for (int i = 0; i < 4; i++) {
        if (client.roomData.players.size() <= i) break;

        sf::Text nickname;
        nickname.setFont(font);
        nickname.setString(client.roomData.players[i].nickname);
        sf::FloatRect textBounds = nickname.getLocalBounds();
        nickname.setPosition(i%2 == 0 ? (window.getSize().x/2)-textBounds.width-50 : (window.getSize().x/2)+50, i<2 ? (window.getSize().y/2)-50 : (window.getSize().y/2)+30);
        window.draw(nickname);

        int j = 0;
        for (const auto& guess : client.roomData.players[i].guessed) {
            sf::RectangleShape square(sf::Vector2f(50,50));
            square.setPosition((i%2 == 0 ? (window.getSize().x/4): (window.getSize().x)-(window.getSize().x/4)) + (j*50) - 200, i<2 ? (window.getSize().y/2)-120 : (window.getSize().y)-100);
            window.draw(square);

            sf::RectangleShape smallSquare(sf::Vector2f(40,40));
            smallSquare.setPosition((i%2 == 0 ? (window.getSize().x/4): (window.getSize().x)-(window.getSize().x/4)) + (j*50) + 5 - 200, (i<2 ? (window.getSize().y/2)-120 : (window.getSize().y)-100) + 5);
            smallSquare.setFillColor( guess == '0' ? sf::Color::Red : sf::Color::Green);
            window.draw(smallSquare);

            j++;
        }
    }

    window.draw(leaveRoomButton);
    window.draw(title);
    window.draw(yLine);
    window.draw(xLine);
}

void GUI::drawLobbyScreen(Client& client) {
    sf::View scrollView(window.getView());
    scrollView.setSize(window.getSize().x, window.getSize().y);
    window.setView(scrollView); 

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
        if (client.rooms[i].playersCount < 4) window.draw(joinButton);

        joinButtons.push_back({joinButton, client.rooms[i].roomId});
    }
}

void GUI::updateInputText() {
    inputText.setString("Input: " + inputBuffer);
}

void GUI::handle_input(sf::Event event, Client& client) {
    if (event.text.unicode == '\r') {
        if (!client.connected) {
            if (!client.connect_to_server(inputBuffer, 8080)) client.error = "Unknown IP";
        } else if (client.nickname.empty()) {
            string cmd = "SET_NICKNAME;" + inputBuffer; 
            client.send_to_server(cmd);
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
    string cmd = "";

    if (client.roomId < 0 && createRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "CREATE_ROOM";
    else if (client.roomId < 0 && refreshButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "ROOM_LIST";    
    else if (client.roomId > -1 && leaveRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "LEAVE_ROOM";    
    else if (client.roomId < 0) {
        for (int i = 0; i < joinButtons.size(); i++) {
            if (joinButtons[i].button.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "JOIN_ROOM;" + to_string(joinButtons[i].id);
        }
    }

    if (!cmd.empty()) client.send_to_server(cmd);
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