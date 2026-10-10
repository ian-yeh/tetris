#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include <stdbool.h>
#include "tetris.h"

void displayBoard(GameState *state, WINDOW *w);
void displayScore(GameState *state, WINDOW *w);
void displayDebugger(GameState *state, WINDOW *w);

#endif
