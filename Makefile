# Example Makefile for OBME Core
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./src
SRC = $(wildcard src/*.cpp src/engine/*.cpp src/io/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = obme-core

default: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)
