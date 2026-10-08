#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include <stdbool.h>
#include "tetris.h"

void render_game(GameState *state, WINDOW *w);
void display_score(GameState *state, WINDOW *w);
void display_debugger(GameState *state, WINDOW *w);

#endif
