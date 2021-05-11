CC=g++
# CFLAGS = -c -Wall -Iinclude/ -std=c++17
CFLAGS = -c -Iinclude/ -std=c++17
LDFLAGS = -L./include/spdlog/build/ -lspdloc -static
LIBS = -lspdloc

default:
	$(MAKE) full

# SRC := $(wildcard src/*.c)
BUILD_DIR := ./build
MAIN_SRC = $(filter-out src/p2c.cpp, $(wildcard src/*.cpp))
P2C_SRC = $(filter-out src/main.cpp, $(wildcard src/*.cpp))

full: $(MAIN_SRC)
	$(CC) $(MAIN_SRC) -o $(BUILD_DIR)/pas2c \
	&& cd build && ./pas2c

p2c: $(P2C_SRC)
	$(CC) $(P2C_SRC) -o $(BUILD_DIR)/p2c

clean:
	$(RM) build/pas2c
