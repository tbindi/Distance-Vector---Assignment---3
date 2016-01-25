#!/bin/bash
tar -xvzf file.tar.gz
g++ Main.cpp Node.cpp Socket.cpp RouteEntry.cpp Advertise.cpp Logger.cpp -pthread -o Main
#g++ -std=c++0x -pthread -o Main Main.cpp Node.cpp Socket.cpp RouteEntry.cpp Advertise.cpp Logger.cpp