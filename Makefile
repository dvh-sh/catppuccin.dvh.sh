CXX ?= g++
build ?= release

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests
TARGET = catppuccin-api
TEST_TARGET = test-runner

SOURCES = $(wildcard $(SRC_DIR)/*.cc)
OBJECTS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SOURCES))

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cc)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cc, $(BUILD_DIR)/test_%.o, $(TEST_SOURCES))

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

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I./tests -c $< -o $@

$(TEST_TARGET): $(TEST_OBJECTS) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS))
	$(CXX) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS)) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: release
	./$(TARGET)

run-debug: debug
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

test-verbose: $(TEST_TARGET)
	./$(TEST_TARGET) -s

test-debug:
	$(MAKE) test build=debug

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

format:
	@echo "==> Formatting C++ source files..."
	@find src/ tests/ -iname "*.cc" -o -iname "*.hpp" | xargs clang-format -i
	@echo "==> Formatting complete."

.PHONY: all release debug run run-debug test test-verbose test-debug clean format
