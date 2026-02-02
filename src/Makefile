# Makefile for C++ OpenGL game

CXX      = g++ -std=c++17 
CXXFLAGS = -std=c++17 -Wall -O2
LIBS     = -lGL -lGLU -lglut

SRC  = main.cpp
OBJ  = $(SRC:.cpp=.o)
TARGET = gameworld

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

