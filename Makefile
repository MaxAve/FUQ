CXX = g++
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TARGET = test/db
all: $(TARGET)
$(TARGET): $(SRC_FILES)
	$(CXX) -std=c++17 $(SRC_FILES) -Iinclude -o $(TARGET)
clean:
	rm -f $(TARGET)
.PHONY: all clean