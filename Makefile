# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = bin
TEST_DIR = test

# Target executable
TARGET = $(BUILD_DIR)/netc_scanner

# Source files
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/scanner.cpp $(SRC_DIR)/token.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target - build the scanner
all: $(BUILD_DIR) $(TARGET)
	@echo "Build complete! Executable: $(TARGET)"

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# Run with a specific file
run: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make run FILE=<input_file>"; \
		echo "Example: make run FILE=test/test_basic.netc"; \
	else \
		./$(TARGET) $(FILE); \
	fi

# Run all tests
test: $(TARGET)
	@echo "Running all test cases..."
	@for testfile in $(TEST_DIR)/*.netc; do \
		echo "\n=== Testing $$testfile ==="; \
		./$(TARGET) $$testfile; \
	done

# Run specific test
test1: $(TARGET)
	./$(TARGET) $(TEST_DIR)/test_basic.netc

test2: $(TARGET)
	./$(TARGET) $(TEST_DIR)/test_operators.netc

test3: $(TARGET)
	./$(TARGET) $(TEST_DIR)/test_keywords.netc

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SRC_DIR)/*.o
	rm -f *_tokens.txt
	@echo "Clean complete!"

# Help target
help:
	@echo "NetC Scanner Makefile"
	@echo "Available targets:"
	@echo "  make          - Build the scanner"
	@echo "  make run FILE=<file> - Run scanner on specific file"
	@echo "  make test     - Run all test cases"
	@echo "  make test1    - Run basic test"
	@echo "  make test2    - Run operator test"
	@echo "  make test3    - Run keyword test"
	@echo "  make clean    - Remove build files"
	@echo "  make help     - Show this help message"

# Phony targets (not actual files)
.PHONY: all run test test1 test2 test3 clean help