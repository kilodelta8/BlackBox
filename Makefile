# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -O2
LDFLAGS = 

# Directories
SRC_DIR = firmware
BIN_DIR = bin

# Target executable name
TARGET = $(BIN_DIR)/witty_manager

# Source files
SRCS = $(SRC_DIR)/witty_manager.cpp

# Default rule
all: $(TARGET)

# Create bin directory and compile
$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

# Run the engine (requires sudo for I2C access)
run: all
	sudo ./$(TARGET)

.PHONY: all clean run
