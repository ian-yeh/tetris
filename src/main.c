#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "display.h"
#include "tetris.h"
#include "main.h"

int main() {
  printf("Welcome to Tetris!\n");

  // start ncurses display
  initDisplay();

  // initializing game windows
  WINDOW *board, *score, *debug;

  // newwin(int rows, int cols, int begin_y, int begin_x)
  board = newwin(BOARD_ROWS+2, (BOARD_COLS*COL_SPACE)+2, 4, 10);
  score = newwin(BOARD_ROWS+2, 20, 4, BOARD_COLS*COL_SPACE + 10 + 5);
  debug = newwin(BOARD_ROWS+2, 40, 4, BOARD_COLS*COL_SPACE + 10 + 25);

  GameState game;
  tetrisInitGame(&game);

  double timeAccumulator = 0.0; // keep track of total time
  double tickInterval = 0.5;
  double lastTime = getCurrentTime();

  while (!game.gameOver) {

    double currentTime = getCurrentTime();

    double dt = currentTime - lastTime;
    lastTime = currentTime;

    // add elapsed time to accumulator
    timeAccumulator += dt;

    // handle inputs
    handleInput(&game);

    while (timeAccumulator >= tickInterval) {
      // move pieces down
      tetrisTick(&game);
      
      timeAccumulator -= tickInterval;
    }

    // rendering game
    displayBoard(&game, board);
    displayScore(&game, score);
    displayDebugger(&game, debug);

    doupdate();

    usleep(16000);
  }

  // cleanup
  endwin();
  printf("Game Over\n");

  return EXIT_SUCCESS;
}


double getCurrentTime() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

void handleInput(GameState *state) {

  // getting input from ncurses's getch
  switch (getch()) {
    case KEY_LEFT:
    case 'h':
      tetrisMove(state, -1);
      break;

    case KEY_RIGHT:
    case 'l':
      tetrisMove(state, 1);
      break;
      
    case KEY_UP:
    case 'k':
      tetrisRotate(state);
      break;

    case KEY_DOWN:
    case 'j':
      tetrisSoftDrop(state);
      break;

    case ' ':
      tetrisHardDrop(state);
      break;

    case 'q':
      state->gameOver = true;
      break;
  }

  return;
}

