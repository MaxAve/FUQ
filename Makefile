CXX = g++
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TARGET = db
all: $(TARGET)
$(TARGET): $(SRC_FILES)
	$(CXX) $(SRC_FILES) -Iinclude -o $(TARGET)
clean:
	rm -f $(TARGET)
.PHONY: all clean