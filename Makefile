BIN_NAME   := rv64emu
BUILD_DIR  := build
BUILD_PATH := $(BUILD_DIR)/$(BIN_NAME)

.phony: all
all: $(BUILD_DIR)
	g++ -o $(BUILD_PATH) src/main.cpp

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

