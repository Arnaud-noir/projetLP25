# Makefile - Gestionnaire de processus multi-hôtes
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2
LDFLAGS = -lncurses

SRC = src/main.c src/options.c src/util.c src/manager.c src/process.c src/network.c src/ui.c src/list.c
OBJ = $(SRC:.c=.o)
BIN = procmanager

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c src/common.h src/util.h src/options.h src/manager.h src/process.h src/network.h src/ui.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
