#include "include/GUI.hpp"

sf::RectangleShape GUI::createButton(const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color, const std::string& text, sf::Text& textElement) {
    sf::RectangleShape button;
    button.setSize(size);
    button.setPosition(position);
    button.setFillColor(color);

    // Add border
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::Black);

    // Set text properties
    textElement.setFont(font);
    textElement.setString(text);
    textElement.setCharacterSize(24); // Adjust text size as needed
    textElement.setFillColor(sf::Color::White); // Text color
    sf::FloatRect textBounds = textElement.getLocalBounds();
    textElement.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    textElement.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);

    return button;
}

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

    leaveRoomButton = createButton(sf::Vector2f(150, 30), sf::Vector2f(window.getSize().x-150, window.getSize().y-30), sf::Color(122,0,0), "Leave", leaveRoomText);

    if (client.roomData.ownsRoom && client.roomData.turnId < 0 && client.roomData.players.size() >= 2) {
        startRoomButton = createButton(sf::Vector2f(150, 30), sf::Vector2f(window.getSize().x-310, window.getSize().y-30), sf::Color(0,122,0), "Start", startRoomText);
        window.draw(startRoomButton);
        window.draw(startRoomText);
    }

    for (int i = 0; i < 4; i++) {
        if (client.roomData.players.size() <= static_cast<size_t>(i)) break;

        if (client.nickname == client.roomData.players[i].nickname) {
            sf::RectangleShape playerArea(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
            playerArea.setPosition(i % 2 == 0 ? 0 : window.getSize().x / 2, i < 2 ? 0 : window.getSize().y / 2);
            playerArea.setFillColor(sf::Color(255, 255, 0, 100));
            window.draw(playerArea);
        }

        if (client.roomData.turnId == i) {
            sf::RectangleShape playerArea(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
            playerArea.setPosition(i % 2 == 0 ? 0 : window.getSize().x / 2, i < 2 ? 0 : window.getSize().y / 2);
            playerArea.setFillColor(sf::Color(0, 255, 0, 50));

            window.draw(playerArea);
        }

        sf::Texture texture;
        sf::Sprite sprite;
        std::string filename = "assets/" + std::to_string(min(client.roomData.players[i].missed,6)) + ".png";
        if (!texture.loadFromFile(filename)) {
            std::cerr << "Nie można załadować pliku: " << filename << std::endl;
            return ;
        }

        sprite.setScale(0.5f, 0.5f);
        sprite.setTexture(texture);
        sprite.setPosition((i%2 == 0 ? (window.getSize().x/4)-100: (window.getSize().x)-(window.getSize().x/4)-100), i<2 ? (window.getSize().y/2)-350 : (window.getSize().y)-350);
        window.draw(sprite);

        sf::Text nickname;
        nickname.setFont(font);
        nickname.setString(client.roomData.players[i].nickname + ((client.nickname == client.roomData.players[i].nickname) ? " (YOU)" : ""));
        sf::FloatRect textBounds = nickname.getLocalBounds();
        nickname.setPosition(i%2 == 0 ? (window.getSize().x/2)-textBounds.width-50 : (window.getSize().x/2)+50, i<2 ? (window.getSize().y/2)-50 : (window.getSize().y/2)+30);
        window.draw(nickname);

        int j = 0;
        for (const auto& guess : client.roomData.players[i].guessed) {
            sf::RectangleShape square(sf::Vector2f(30, 30));
            square.setPosition((i % 2 == 0 ? (window.getSize().x / 4) : (window.getSize().x) - (window.getSize().x / 4)) + (j * 35) - 150, i < 2 ? (window.getSize().y / 2) - 100 : (window.getSize().y) - 80);
            square.setFillColor(sf::Color::Black);
            window.draw(square);

            sf::RectangleShape smallSquare(sf::Vector2f(20, 20));
            smallSquare.setPosition((i % 2 == 0 ? (window.getSize().x / 4) : (window.getSize().x) - (window.getSize().x / 4)) + (j * 35) + 5 - 150, (i < 2 ? (window.getSize().y / 2) - 100 : (window.getSize().y) - 80) + 5);
            smallSquare.setFillColor(guess == '0' ? sf::Color::Red : sf::Color::Green);
            window.draw(smallSquare);

            if (client.nickname == client.roomData.players[i].nickname && client.userGuessed[j] != '\0') {
                sf::Text guessText;
                guessText.setFont(font);
                guessText.setString(std::string(1, client.userGuessed[j]));
                guessText.setCharacterSize(16);
                guessText.setFillColor(sf::Color::White);
                sf::FloatRect textBounds = guessText.getLocalBounds();
                guessText.setPosition(smallSquare.getPosition().x + (smallSquare.getSize().x / 2) - (textBounds.width / 2), smallSquare.getPosition().y + (smallSquare.getSize().y / 2) - (textBounds.height / 2)+20);
                window.draw(guessText);
            }

            j++;
        }
    }

    window.draw(leaveRoomButton);
    window.draw(leaveRoomText);
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

    createRoomButton = createButton(sf::Vector2f(200, 50), sf::Vector2f(window.getSize().x-470, 50), sf::Color(0, 122, 0), "Create Room", createRoomText);
    refreshButton = createButton(sf::Vector2f(200, 50), sf::Vector2f(window.getSize().x-250, 50), sf::Color(0, 0, 122), "Refresh", refreshText);

    window.draw(title);
    window.draw(createRoomButton);
    window.draw(createRoomText);
    window.draw(refreshButton);
    window.draw(refreshText);

    const int roomHeight = 50;
    const int maxRoomsVisible = window.getSize().y / roomHeight; 

    if (client.rooms.size() > static_cast<size_t>(maxRoomsVisible)) {
        scrollOffset = std::max(0, scrollOffset); 
    }

    for (int i = scrollOffset; static_cast<size_t>(i) < client.rooms.size(); i++) {
        sf::Text roomText;
        roomText.setFont(font);
        roomText.setString("Room #" + std::to_string(client.rooms[i].roomId));
        roomText.setPosition(20, 130 + (i - scrollOffset) * roomHeight); 
        sf::Text playerCount;
        playerCount.setFont(font);
        playerCount.setString("Players: " + std::to_string(client.rooms[i].playersCount) + "/4");
        playerCount.setPosition(200, 130 + (i - scrollOffset) * roomHeight); 

        sf::Text joinText;
        sf::RectangleShape joinButton;
        joinButton = createButton(sf::Vector2f(100, roomHeight-20), sf::Vector2f(400, 140 + (i - scrollOffset) * roomHeight), sf::Color(0,122,0), "Join", joinText);

        window.draw(roomText);
        window.draw(playerCount);
        if (client.rooms[i].playersCount < 4) {
            window.draw(joinButton);
            window.draw(joinText);
        }

        joinButtons.push_back({joinButton, client.rooms[i].roomId});
    }
}

void GUI::updateInputText() {
    inputText.setString("Input: " + inputBuffer);
}

void GUI::handle_input(sf::Event event, Client& client) {
    if (client.roomId < 0) {
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
    } else if (client.roomData.turnId == client.roomData.playerPosition) {
        if ((event.text.unicode >= 'A' && event.text.unicode <= 'Z') ||  (event.text.unicode >= 'a' && event.text.unicode <= 'z')) {
            string cmd = "GUESS;" + std::string(1, static_cast<char>(event.text.unicode));
            client.send_to_server(cmd);
        }
    }

    updateInputText();
}

void GUI::handle_clicks(Client& client) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    string cmd = "";

    if (client.roomId < 0 && createRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "CREATE_ROOM";
    else if (client.roomId < 0 && refreshButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "ROOM_LIST";    
    else if (client.roomId > -1 && leaveRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "LEAVE_ROOM";    
    else if (client.roomId > -1 && startRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) cmd = "START_ROOM";    
    else if (client.roomId < 0) {
        for (size_t i = 0; i < joinButtons.size(); i++) {
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

void GUI::drawResults(Client& client) {
    title.setString("Game Results");
    window.draw(title);

    for (size_t i = 0; i < client.winners.size()-1; i++) {
        sf::Text name;
        name.setFont(font);
        name.setString((to_string(i + 1)) + ". " + client.winners[i]);
        name.setCharacterSize(28);
        name.setFillColor(sf::Color::White);
        name.setStyle(sf::Text::Bold);
        name.setPosition(window.getSize().x / 2 - name.getLocalBounds().width / 2, 80 + i * 50);

        window.draw(name);
    }

    leaveRoomButton = createButton(sf::Vector2f(150, 50), sf::Vector2f(window.getSize().x / 2 - 75, window.getSize().y - 100), sf::Color(122, 0, 0), "Leave", leaveRoomText);
    window.draw(leaveRoomButton);
    window.draw(leaveRoomText);
}