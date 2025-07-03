CXX ?= g++
build ?= release

SRC_DIR = src
BUILD_DIR = build
TARGET = catppuccin-api
SOURCES = $(wildcard $(SRC_DIR)/*.cc)
OBJECTS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SOURCES))

COMMON_CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I./src
COMMON_LDFLAGS = -lpthread

ifeq ($(build),release)
    BUILD_CXXFLAGS = -O3 -DNDEBUG
    BUILD_LDFLAGS = -s
else
    BUILD_CXXFLAGS = -O0 -g -DDEBUG
    BUILD_LDFLAGS =
endif

ifeq ($(build),release)
    ifeq ($(findstring g++,$(CXX)),g++)
        COMPILER_CXXFLAGS = -march=native -mtune=native -flto -ffast-math -funroll-loops -finline-functions
        COMPILER_LDFLAGS = -static-libgcc -static-libstdc++ -Wl,--gc-sections -flto
    else ifeq ($(findstring clang++,$(CXX)),clang++)
        COMPILER_CXXFLAGS = -march=native -mtune=native -flto
        COMPILER_LDFLAGS = -Wl,-dead_strip
    endif
endif

CXXFLAGS = $(COMMON_CXXFLAGS) $(BUILD_CXXFLAGS) $(COMPILER_CXXFLAGS)
LDFLAGS = $(COMMON_LDFLAGS) $(BUILD_LDFLAGS) $(COMPILER_LDFLAGS)

all: $(TARGET)

release:
	$(MAKE) all build=release

debug:
	$(MAKE) all build=debug

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: release
	./$(TARGET)

run-debug: debug
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all release debug run run-debug clean
