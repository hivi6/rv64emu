BIN_NAME   := rv64emu
BUILD_DIR  := build
BUILD_PATH := $(BUILD_DIR)/$(BIN_NAME)

CPP_FLAGS := -std=c++23 -Wall -Wextra -g

INC_DIR := include
SRC_DIR := src

HPP_FILES := $(wildcard $(INC_DIR)/*.hpp)
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_FILES))

.PHONY: all clean
all: $(BUILD_PATH)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_PATH): $(BUILD_DIR) $(OBJ_FILES) $(HPP_FILES)
	g++ $(OBJ_FILES) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	g++ -I$(INC_DIR) $(CPP_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(OBJ_FILES:.o=.d)

