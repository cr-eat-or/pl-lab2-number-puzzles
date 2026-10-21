CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra
OPT    ?= -O0
FILE   ?= tests/puzzles_small.txt
REPEATS ?= 3
VER    ?= v0
BUILD   = build
EXE     = $(if $(OS),.exe,)

SRC_LIB = src/puzzle.c src/solver.c

.PHONY: all build test bench profile clean

all: build

build: $(BUILD)/rebus$(EXE)

$(BUILD)/rebus$(EXE): src/main.c $(SRC_LIB) src/puzzle.h src/solver.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(OPT) -Isrc src/main.c $(SRC_LIB) -o $@

$(BUILD)/check$(EXE): tests/check.c $(SRC_LIB) src/puzzle.h src/solver.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(OPT) -Isrc tests/check.c $(SRC_LIB) -o $@

test: $(BUILD)/check$(EXE)
	./$(BUILD)/check tests/puzzles_small.txt

$(BUILD)/bench$(EXE): bench/bench.c $(SRC_LIB) src/puzzle.h src/solver.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(OPT) -Isrc bench/bench.c $(SRC_LIB) -o $@

# make bench VER=v1 FILE=tests/puzzles_big.txt REPEATS=5 -> results/v1.csv
bench: $(BUILD)/bench$(EXE)
	mkdir -p results
	./$(BUILD)/bench $(FILE) $(REPEATS) > results/$(VER).csv

profile:
	@echo "TODO: цель profile (пункт 2.1)"

clean:
	rm -rf $(BUILD) gmon.out
