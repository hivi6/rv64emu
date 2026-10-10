CPP_FLAGS := -std=c++23 -Wall -Wextra -g

BUILD_DIR  := build
OBJ_DIR    := build/obj

INC_DIR := include
SRC_DIR := src

HPP_FILES := $(wildcard $(INC_DIR)/*.hpp)
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)

MAIN_CPP := $(SRC_DIR)/main.cpp
MAIN_OBJ := $(OBJ_DIR)/main.o
MAIN_BIN := $(BUILD_DIR)/rv64emu

CORE_CPP := $(filter-out $(MAIN_CPP),$(CPP_FILES))
CORE_OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CORE_CPP))

OBJ_FILES := $(CORE_OBJ) $(MAIN_OBJ)

.PHONY: all clean
all: $(MAIN_BIN)

clean:
	rm -rf $(BUILD_DIR)

$(MAIN_BIN): $(BUILD_DIR) $(CORE_OBJ) $(MAIN_OBJ) $(HPP_FILES)
	g++ $(CORE_OBJ) $(MAIN_OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	g++ -I$(INC_DIR) $(CPP_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

-include $(OBJ_FILES:.o=.d)

