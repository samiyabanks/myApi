CXX := clang++
CXXFLAGS := -Wall -Wextra -std=c++17 -pthread -g
CPPFLAGS := -Iinclude -I/opt/homebrew/include -I/opt/homebrew/opt/cpp-httplib/include

BIN := bin
SRC := src
TARGET := $(BIN)/main

.PHONY: all run clean

all: $(TARGET)

$(BIN):
	mkdir -p $@

$(TARGET): $(SRC)/main.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BIN)