CPP_FLAGS := -std=c++23 -Wall -Wextra -g

BUILD_DIR  := build
OBJ_DIR    := build/obj

INC_DIR  := include
SRC_DIR  := src
TEST_DIR := tests

HPP_FILES := $(wildcard $(INC_DIR)/*.hpp)
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)

MAIN_CPP := $(SRC_DIR)/main.cpp
MAIN_OBJ := $(OBJ_DIR)/main.o
MAIN_BIN := $(BUILD_DIR)/rv64emu

RV64_TEST_CPP := $(TEST_DIR)/rv64-test.cpp
RV64_TEST_OBJ := $(OBJ_DIR)/rv64-test.o
RV64_TEST_BIN := $(BUILD_DIR)/rv64-test

# Scripts for running the tests
RV64I_TEST_SH  := $(TEST_DIR)/test-i.sh

UART_TEST_CPP := $(TEST_DIR)/uart/uart-test.cpp
UART_TEST_OBJ := $(OBJ_DIR)/uart-test.o
UART_TEST_BIN := $(BUILD_DIR)/uart-test
UART_TEST_SH  := $(TEST_DIR)/test-uart.sh

CORE_CPP := $(filter-out $(MAIN_CPP),$(CPP_FILES))
CORE_OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CORE_CPP))

OBJ_FILES := $(CORE_OBJ) $(MAIN_OBJ) $(RV64I_TEST_OBJ)

.PHONY: all clean test test-i test-uart
all: $(MAIN_BIN)

clean:
	rm -rf $(BUILD_DIR)

test: test-i test-uart

test-i: $(RV64_TEST_BIN)
	bash $(RV64I_TEST_SH)

test-uart: $(UART_TEST_BIN)
	bash $(UART_TEST_SH)

$(MAIN_BIN): $(CORE_OBJ) $(MAIN_OBJ) $(HPP_FILES) | $(BUILD_DIR)
	g++ $(CORE_OBJ) $(MAIN_OBJ) -o $@

$(RV64_TEST_BIN): $(CORE_OBJ) $(RV64_TEST_OBJ) $(HPP_FILES) | $(BUILD_DIR)
	g++ $(CORE_OBJ) $(RV64_TEST_OBJ) -o $@

$(RV64_TEST_OBJ): $(RV64_TEST_CPP) | $(OBJ_DIR)
	g++ -I$(INC_DIR) $(CPP_FLAGS) -MMD -MP -c $< -o $@

$(UART_TEST_BIN): $(CORE_OBJ) $(UART_TEST_OBJ) $(HPP_FILES) | $(BUILD_DIR)
	g++ $(CORE_OBJ) $(UART_TEST_OBJ) -o $@

$(UART_TEST_OBJ): $(UART_TEST_CPP) | $(OBJ_DIR)
	g++ -I$(INC_DIR) $(CPP_FLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	g++ -I$(INC_DIR) $(CPP_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

-include $(OBJ_FILES:.o=.d)

