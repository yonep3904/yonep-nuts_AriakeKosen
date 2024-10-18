CXX = g++
CXXFLAGS = -lws2_32 -Iinclude -Ilib -O3 -mtune=native -march=native -mfpmath=both
LDFLAGS = -Llib

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
LIB_DIR = lib
TARGET = yonep-nuts

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# ルール: 全体ビルド
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS) -lws2_32

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ルール: クリーン
clean:
	@del.bat $(TARGET) $(OBJ_DIR)
.PHONY: all clean