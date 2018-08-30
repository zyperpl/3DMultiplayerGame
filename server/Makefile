SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS  := -O3 -ggdb
CPPFLAGS := -O3 -ggdb

server: $(OBJ_FILES)
	   g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	   g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf obj/*
