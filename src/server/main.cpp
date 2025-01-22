#include "include/ServerManager.hpp"

int main() {
    ServerManager server;
    server.port = 8080;

    server.start();
}