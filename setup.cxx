CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -ggdb

BIN := bin
LIB := lib
SRC := $(wildcard src/*.cpp)
CPPFLAGS := -Iinclude -Ilib
LDFLAGS :=
LDLIBS :=
EXECUTABLE := main

.PHONY: all run clean

all: $(BIN)/$(EXECUTABLE)

run: clean all
	./$(BIN)/$(EXECUTABLE)

$(BIN):
	mkdir -p $@

$(BIN)/$(EXECUTABLE): $(SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rf $(BIN)