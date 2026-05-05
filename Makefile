CXX = /usr/bin/g++
CXXFLAGS = -std=c++11 -Wall -pthread -isysroot `xcrun --show-sdk-path`

all: server

server: src/server.cpp src/go_logic.cpp src/auth.cpp
	$(CXX) $(CXXFLAGS) -o server src/server.cpp src/go_logic.cpp src/auth.cpp

clean:
	rm -f server
