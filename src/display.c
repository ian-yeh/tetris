#include <ncurses.h>
#include "tetris.h"
#include "main.h"

void displayBoard(GameState *state, WINDOW *w) {
  werase(w);
  int i, j, k;

  // creating box for the game board
  box(w, 0, 0);

  // drawing the board
  for (i = 0; i < BOARD_ROWS; i++) {
    // moves the cursor to the given row
    wmove(w, 1 + i, 1);

    for (j = 0; j < BOARD_COLS; j++) {
      if (state->board[i][j]) {
        wattron(w, COLOR_PAIR(state->board[i][j]));

        for (k = 0; k < COL_SPACE; ++k) waddch(w, ' ');

        wattroff(w, COLOR_PAIR(state->board[i][j]));
      } else {
        //wattron(w, COLOR_PAIR(5));
        for (k = 0; k < COL_SPACE; ++k) waddch(w, ' ');
        //wattroff(w, COLOR_PAIR(5));
      }
    }
  }

  wnoutrefresh(w);
}

void displayScore(GameState *state, WINDOW *w) {
  werase(w);
  box(w, 0, 0);

  mvwprintw(w, 1, 2, "Score: %d", state->points);
  mvwprintw(w, 2, 2, "Level: %d", state->level);
  mvwprintw(w, 3, 2, "Lines: %d", state->linesRemaining);

  wnoutrefresh(w);
}


void displayDebugger(GameState *state, WINDOW *w) {
  werase(w);

  box(w, 0, 0);
  mvwprintw(w, 1, 2, "Debugger");

  mvwprintw(w, 2, 2, "Piece Row: %d", state->falling.loc.row);
  mvwprintw(w, 4, 2, "Ticks Until Falling: %d", state->ticksUntilGravity);
  //mvwprintw(w, 5, 2, "Tetris Fits: %d", tetrisFits(state, &state->falling));

  //mvwprintw(w, 7, 2, "Falling Block Row: %d", tetrisFits(state, &state->falling));
  //mvwprintw(w, 8, 2, "Falling Block Col: %d", tetrisFits(state, &state->falling));

  wnoutrefresh(w);
}

