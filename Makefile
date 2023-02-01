CC := gcc
CFLAGS = -Wextra -std=c99
CFLAGS += -ldl -lpthread -lSDL2 -lSDL2_ttf

SRC  = $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ  = $(SRC:.c=.o)
BIN = bin

.PHONY: build clean

build: dirs exe

dirs:
	mkdir -p ./$(BIN)

run: clean exe
	$(BIN)/exe

exe: $(OBJ)
	$(CC) -o $(BIN)/exe $^ $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN)/* $(OBJ)