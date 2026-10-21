CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra
OPT    ?= -O0
BUILD   = build

SRC_LIB = src/puzzle.c src/solver.c

.PHONY: all build test bench profile clean

all: build

build:
	@echo "TODO: цель build (пункты 1.1-1.3)"

test:
	@echo "TODO: цель test (пункт 1.3)"

bench:
	@echo "TODO: цель bench (пункт 1.4)"

profile:
	@echo "TODO: цель profile (пункт 2.1)"

clean:
	rm -rf $(BUILD) gmon.out
