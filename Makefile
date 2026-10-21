CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra
OPT    ?= -O0
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

bench:
	@echo "TODO: цель bench (пункт 1.4)"

profile:
	@echo "TODO: цель profile (пункт 2.1)"

clean:
	rm -rf $(BUILD) gmon.out
