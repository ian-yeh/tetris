CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncurses
SRC = src/main.c src/tetris.c src/display.c
TARGET = main

all: $(TARGET) run

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
