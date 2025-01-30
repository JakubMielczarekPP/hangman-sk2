mkdir build
c++ src/client/*.cpp -Wextra -Wall -o build/client -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
c++ src/server/*.cpp -Wextra       -Wall -o build/server
cp -r src/client/assets  build/assets