CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
LIBS = -lGL -lGLU -lglut

SRCS = main.cpp spider.cpp tree.cpp goldpiece.cpp diamondpiece.cpp tower.cpp slime.cpp vector3.cpp synthesizer.cpp wasp.cpp treasuretower.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = gameworld

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.wav

.PHONY: clean
