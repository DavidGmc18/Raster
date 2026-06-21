export SOURCE_DIR := $(abspath .)
export BUILD_DIR := $(abspath build)
export SRC_DIR := $(abspath src)

export C := gcc -DDEBUG
export CFLAGS := -g -O3 -march=native -flto -fno-math-errno -fno-trapping-math -Wall -Wextra -fdiagnostics-color=always
export INCLUDES := -I. -I$(SRC_DIR)
export LDFLAGS := -lSDL3 -lm

export BINARY := $(BUILD_DIR)/out.bin