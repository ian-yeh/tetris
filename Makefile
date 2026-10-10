CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncurses
CHECK_CHANGE = src/main.c src/tetris.c src/display.c src/main.h src/tetris.h src/display.h 
SRC = src/main.c src/tetris.c src/display.c
TARGET = main

all: $(TARGET) run

$(TARGET): $(CHECK_CHANGE)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
