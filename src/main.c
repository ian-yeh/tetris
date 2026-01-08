#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>

#include "tetris.h"

#define COL_SPACE 2


void render_game(GameState *state, WINDOW *w);
double get_current_time();
void handle_input(GameState *state);
void init_display();
void display_score(GameState *state, WINDOW *w);
void display_debugger(GameState *state, WINDOW *w);

int main() {
  printf("Welcome to Tetris!\n");

  // start ncurses display
  init_display();

  // initializing game windows
  WINDOW *board, *score, *debug;
  board = newwin(ROWS+2, (COLS*COL_SPACE)+2, 4, 20);
  score = newwin(15, 25, 4, COLS*COL_SPACE + 40);
  debug = newwin(30, 50, 4, COLS*COL_SPACE + 70);

  GameState game;

  tetris_init(&game);

  double time_accumulator = 0.0; // keep track of total time
  double tick_interval = 0.5;
  double last_time = get_current_time();

  while (!game.game_over) {

    double current_time = get_current_time();

    double dt = current_time - last_time;
    last_time = current_time;

    // add elapsed time to accumulator
    time_accumulator += dt;

    // handle inputs
    handle_input(&game);

    while (time_accumulator >= tick_interval) {
      // move pieces down
      tetris_tick(&game);
      
      time_accumulator -= tick_interval;
    }

    // rendering game
    render_game(&game, board);
    display_score(&game, score);
    display_debugger(&game, debug);

    doupdate();

    usleep(16000);
  }

  // cleanup
  endwin();
  printf("Game Over\n");

  return 2;
}

void render_game(GameState *state, WINDOW *w) {
  werase(w);
  int i, j, k;

  // creating box for the game board
  box(w, 0, 0);

  // drawing the board
  for (i = 0; i < ROWS; i++) {
    // moves the cursor to the given row
    wmove(w, 1 + i, 1);

    for (j = 0; j < COLS; j++) {
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

double get_current_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

void handle_input(GameState *state) {

  // getting input from ncurses's getch
  switch (getch()) {
    case KEY_LEFT:
      tetris_move(state, -1);
      break;

    case KEY_RIGHT:
      tetris_move(state, 1);
      break;
      
    case KEY_UP:
      tetris_rotate(state);
      break;

    case KEY_DOWN:
      tetris_soft_drop(state);
      break;

    case ' ':
      tetris_hard_drop(state);
      break;

    case 'q':
      state->game_over = true;
      break;
  }

  return;
}

void init_display() {
  // initializing ncurses
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  start_color();

  // initializing colour pairs
  init_pair(1, COLOR_RED, COLOR_RED); // S piece
  init_pair(2, COLOR_GREEN, COLOR_GREEN); // J piece
  init_pair(3, COLOR_BLUE, COLOR_BLUE); // I piece
  init_pair(4, COLOR_YELLOW, COLOR_YELLOW); // square
  init_pair(5, COLOR_CYAN, COLOR_CYAN); // L piece
  init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA); // T piece
  init_pair(7, COLOR_WHITE, COLOR_WHITE);

  return;
}

void display_score(GameState *state, WINDOW *w) {
  werase(w);
  box(w, 0, 0);

  mvwprintw(w, 1, 2, "Score: %d", state->points);
  mvwprintw(w, 2, 2, "Level: %d", state->level);
  mvwprintw(w, 3, 2, "Lines: %d", state->lines_remaining);

  wnoutrefresh(w);
}


void display_debugger(GameState *state, WINDOW *w) {
  werase(w);

  box(w, 0, 0);
  mvwprintw(w, 1, 2, "Debugger");

  mvwprintw(w, 2, 2, "Piece Row: %d", state->falling.loc.row);
  mvwprintw(w, 4, 2, "Ticks Until Falling: %d", state->ticks_until_gravity);
  //mvwprintw(w, 5, 2, "Tetris Fits: %d", tetris_fits(state, &state->falling));

  //mvwprintw(w, 7, 2, "Falling Block Row: %d", tetris_fits(state, &state->falling));
  //mvwprintw(w, 8, 2, "Falling Block Col: %d", tetris_fits(state, &state->falling));

  wnoutrefresh(w);
}

