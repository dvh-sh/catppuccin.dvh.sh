CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -mtune=native \
           -flto -ffast-math -funroll-loops -finline-functions \
           -DNDEBUG -fno-exceptions -fno-rtti
LDFLAGS = -static-libgcc -static-libstdc++ -s -Wl,--gc-sections -flto
LIBS = -lpthread

SRC_DIR = src
BUILD_DIR = build
SOURCES = $(wildcard $(SRC_DIR)/*.cc)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)
TARGET = catppuccin-api

# Release build (default)
all: release

# Debug build
debug: CXXFLAGS = -std=c++17 -Wall -Wextra -O0 -g -DDEBUG
debug: LDFLAGS = 
debug: $(TARGET)

# Release build
release: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I./include -I./src -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS) $(LDFLAGS)
	strip --strip-all $(TARGET) 2>/dev/null || true

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all debug release run clean install
