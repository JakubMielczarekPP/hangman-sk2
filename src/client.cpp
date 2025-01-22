#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <thread>

#define PORT 8080

// Funkcja nasłuchująca wiadomości od serwera
void listenToServer(sf::TcpSocket& socket, sf::Text& serverText) {
    char buffer[1024];
    std::size_t received;

    while (true) {
        if (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
            serverText.setString("Server: " + std::string(buffer, received));
        } else {
            serverText.setString("Disconnected from server.");
            break;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hangman Client");

    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::Text title("Enter Server IP:", font, 24);
    title.setPosition(20, 20);

    sf::Text inputText("", font, 20);
    inputText.setPosition(20, 100);

    sf::Text serverText("", font, 18);
    serverText.setPosition(20, 200);

    std::string inputBuffer;
    sf::TcpSocket socket;
    bool connected = false;

    // Przycisk GUI
    sf::RectangleShape createRoomButton(sf::Vector2f(200, 50));
    createRoomButton.setPosition(20, 300);
    createRoomButton.setFillColor(sf::Color::Green);

    sf::Text createRoomText("Create Room", font, 20);
    createRoomText.setPosition(40, 310);

    sf::RectangleShape listRoomsButton(sf::Vector2f(200, 50));
    listRoomsButton.setPosition(20, 400);
    listRoomsButton.setFillColor(sf::Color::Blue);

    sf::Text listRoomsText("List Rooms", font, 20);
    listRoomsText.setPosition(50, 410);

    sf::RectangleShape joinRoomButton(sf::Vector2f(200, 50));
    joinRoomButton.setPosition(20, 500);
    joinRoomButton.setFillColor(sf::Color::Yellow);

    sf::Text joinRoomText("Join Room (ID)", font, 20);
    joinRoomText.setPosition(30, 510);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (!connected) {
                // Wpisywanie adresu IP
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\r') {  // Enter
                        if (socket.connect(inputBuffer, PORT) == sf::Socket::Done) {
                            connected = true;
                            std::thread(listenToServer, std::ref(socket), std::ref(serverText)).detach();
                            serverText.setString("Connected to " + inputBuffer);
                        } else {
                            serverText.setString("Failed to connect to " + inputBuffer);
                        }
                        inputBuffer.clear();
                    } else if (event.text.unicode == '\b') {  // Backspace
                        if (!inputBuffer.empty()) inputBuffer.pop_back();
                    } else {
                        inputBuffer += static_cast<char>(event.text.unicode);
                    }
                    inputText.setString("Input: " + inputBuffer);
                }
            } else {
                // Obsługa kliknięć przycisków
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (createRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        std::string command = "CREATE_ROOM";
                        socket.send(command.c_str(), command.size()+1);
                    } else if (listRoomsButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        std::string command = "LIST_ROOMS";
                        socket.send(command.c_str(), command.size());
                    } else if (joinRoomButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        std::string roomId;
                        std::cout << "Enter Room ID: ";
                        std::cin >> roomId;
                        std::string command = "JOIN_ROOM " + roomId;
                        socket.send(command.c_str(), command.size());
                    }
                }
            }
        }

        window.clear();

        if (!connected) {
            window.draw(title);
            window.draw(inputText);
        } else {
            window.draw(serverText);

            window.draw(createRoomButton);
            window.draw(createRoomText);

            window.draw(listRoomsButton);
            window.draw(listRoomsText);

            window.draw(joinRoomButton);
            window.draw(joinRoomText);
        }

        window.display();
    }

    return 0;
}
