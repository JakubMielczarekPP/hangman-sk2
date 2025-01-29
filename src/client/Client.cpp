#include "include/Client.hpp"

bool Client::connect_to_server(string& serverIp, int port) {
    if (socket.connect(serverIp, port) == sf::Socket::Done) {
        connected = true;
        scene = 1;

        //lista pokoi
        string cmd = "ROOM_LIST";
        send_to_server(cmd);

        return true;
    }

    return false;
}

void Client::send_to_server(string& message) {
    if (connected) socket.send(message.c_str(), message.size()+1);
}

void Client::receive_message() {
    char data[1024];
    std::size_t received;
    sf::Socket::Status status = socket.receive(data, sizeof(data), received);

    if (status == sf::Socket::Done) {
        std::string message(data, received); 
        if (message.empty()) return;
        
        if (message.find("ROOM_LIST") == 0) {
            process_room_list(message);
            return;
        }

    } 
}

void Client::process_room_list(string& message) {
    rooms.clear();

    std::cout<<message<<std::endl;
    std::stringstream ss(message);
    std::string token;

    std::getline(ss, token, ';');

    // Przetwarzamy kolejne pokoje
    while (std::getline(ss, token, ';')) {
        std::stringstream roomData(token);
        std::string roomIdStr, playersCountStr;

        // Oddzielamy roomId i playersCount
        std::getline(roomData, roomIdStr, ',');
        std::getline(roomData, playersCountStr, ',');

        // Tworzymy obiekt Room i przypisujemy odpowiednie wartości
        Room room;
        room.roomId = std::stoi(roomIdStr);  // Konwersja id na int
        room.playersCount = std::stoi(playersCountStr);  // Konwersja playersCount na int

        // Dodajemy do wektora rooms
        rooms.push_back(room);
    }

    // Debugowanie: wyświetlamy zawartość rooms
    std::cout << "Rooms:" << std::endl;
    for (const Room& room : rooms) {
        std::cout << "Room ID: " << room.roomId << ", Players: " << room.playersCount << std::endl;
    }
}