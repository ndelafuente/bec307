CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++11

SRC_FILES = echo-server.cpp

TARGETS = echo-server

all: $(TARGETS)

echo-sever: $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(TARGETS)
