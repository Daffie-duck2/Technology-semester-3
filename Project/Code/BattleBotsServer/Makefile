CXX=g++
CXXFLAGS=-Wall -Werror -Wextra -pedantic -ggdb -O0 -std=c++11 -Iproduct
LDDFLAGS=-lpthread

SOURCES=$(wildcard src/*.cpp)
HEADERS=$(wildcard src/*.h)
TARGET=BattleBotsServer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDDFLAGS)

clean:
	@rm -f $(TARGET)