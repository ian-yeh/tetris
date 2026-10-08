#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "tetris.h"

/*   DEFINING ARRAYS   */

// Each piece has 4 orientations, each rotation is a 4x4 grid, 7 pieces total
static const int PIECES[NUM_TETROMINOS][NUM_ORIENTATIONS][4][4] = {
    // I, 0 
    {
        {{0,0,0,0}, {3,3,3,3}, {0,0,0,0}, {0,0,0,0}},  
        {{0,0,3,0}, {0,0,3,0}, {0,0,3,0}, {0,0,3,0}},  
        {{0,0,0,0}, {0,0,0,0}, {3,3,3,3}, {0,0,0,0}},  
        {{0,3,0,0}, {0,3,0,0}, {0,3,0,0}, {0,3,0,0}}  
    },
    // square, 1 
    {
        {{0,0,0,0}, {0,4,4,0}, {0,4,4,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,4,4,0}, {0,4,4,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,4,4,0}, {0,4,4,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,4,4,0}, {0,4,4,0}, {0,0,0,0}}
    },
    // T, 2 
    {
        {{0,0,0,0}, {0,6,0,0}, {6,6,6,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,6,0,0}, {0,6,6,0}, {0,6,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {6,6,6,0}, {0,6,0,0}},
        {{0,0,0,0}, {0,6,0,0}, {6,6,0,0}, {0,6,0,0}}
    },
    // S, 3 
    {
        {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,1,1,0}, {1,1,0,0}},
        {{0,0,0,0}, {1,0,0,0}, {1,1,0,0}, {0,1,0,0}}
    },
    // Z, 4 
    {
        {{0,0,0,0}, {2,2,0,0}, {0,2,2,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,2,0}, {0,2,2,0}, {0,2,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {2,2,0,0}, {0,2,2,0}},
        {{0,0,0,0}, {0,2,0,0}, {2,2,0,0}, {2,0,0,0}}
    },
    // J, 5 
    {
        {{0,0,0,0}, {7,0,0,0}, {7,7,7,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,7,7,0}, {0,7,0,0}, {0,7,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {7,7,7,0}, {0,0,7,0}},
        {{0,0,0,0}, {0,7,0,0}, {0,7,0,0}, {7,7,0,0}}
    },
    // L, 6
    {
        {{0,0,0,0}, {0,0,2,0}, {2,2,2,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,2,0,0}, {0,2,0,0}, {0,2,2,0}},
        {{0,0,0,0}, {0,0,0,0}, {2,2,2,0}, {2,0,0,0}},
        {{0,0,0,0}, {2,2,0,0}, {0,2,0,0}, {0,2,0,0}}
    }
};

// gravity becomes faster for increasing levels
int gravity_levels[10] = {2, 9, 8, 7, 6, 5, 4, 3, 2, 1};

/*   HELPER FUNCTIONS FOR BLOCKS  */


static int random_tetromino(void) {
  return rand() % NUM_TETROMINOS;
}

static void tetris_new_falling(GameState *state)
{
  // Put in a new falling tetromino.
  state->falling = state->next;
  state->next.type = random_tetromino();
  state->next.rotation = 0;
  state->next.loc.row = 0;
  state->next.loc.col = COLS/2 - 2;
  state->ticks_until_gravity = gravity_levels[state->level-1];
}

//============================= GAME TURN FUNCTIONS ===============================

// putting the block back onto the board
void tetris_put(GameState *state, Piece *block) {
  // right now, putting a 1x1 block
  int row = block->loc.row;
  int col = block->loc.col;

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {

        // Skip empty cells
        if (PIECES[block->type][block->rotation][i][j] == 0) continue;

        int board_y = row + i;
        int board_x = col + j;

        if (board_x >= 0 && board_x < COLS && 
          board_y >= 0 && board_y < ROWS) {
          state->board[board_y][board_x] = PIECES[block->type][block->rotation][i][j];
        }

      }
    }

}

// removing a block from the board
void tetris_remove(GameState *state, Piece *block) {
  int row = block->loc.row;
  int col = block->loc.col;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {

      // Skip empty cells
      if (PIECES[block->type][block->rotation][i][j] == 0) continue;

      int board_y = row + i;
      int board_x = col + j;

      // checking bounds
      if (board_x >= 0 && board_x < COLS && 
        board_y >= 0 && board_y < ROWS) {
        state->board[board_y][board_x] = 0;
      }

    }
  }

}

// checking if the block fits into the current board
bool tetris_fits(GameState *state, Piece *block) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {

      // skip if it's an empty piece
      if (PIECES[block->type][block->rotation][i][j] == 0) continue;

      int board_x = block->loc.col + j;
      int board_y = block->loc.row + i;

      // checking if the piece is in bounds 
      if (board_x < 0 || board_x >= COLS ||
        board_y < 0 || board_y >= ROWS) {
        return false;
      }

      // checking if the place to be is a 1
      if (state->board[board_y][board_x] != 0) return false;
    }
  }

  return true;
}

void tetris_gravity(GameState *state) {
  state->ticks_until_gravity--;

  if (state->ticks_until_gravity <= 0)  {

    // removing piece to check if its new position works
    tetris_remove(state, &state->falling);

    state->falling.loc.row++;

    // if the tetris piece "fits" (cannot move any longer), reset and create new block. 
    // true fits means it can move more, false fits mean it cannot move any longer
    if (!tetris_fits(state, &state->falling)) {
      state->falling.loc.row--;
      tetris_put(state, &state->falling);
      tetris_new_falling(state);

    } else {
      tetris_put(state, &state->falling);
      state->ticks_until_gravity = gravity_levels[state->level-1];
    }

  }

}

void tetris_check_lines(GameState *state) {
  for (int i = 0; i < ROWS; ++i) {
    bool full = true;
    for (int j = 0; j < COLS; ++j) {
      if (state->board[i][j] == 0) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; --k) {
        for (int l = 0; l < COLS; ++l) {
          state->board[k][l] = state->board[k-1][l];
        }
      }
      i++;
    }
  }

  return;
}

bool tetris_tick(GameState *state) {
  tetris_gravity(state);
  tetris_check_lines(state);

  return true;
}

//======================= MOVEMENT FUNCTIONS ===============================//

// make the current piece move left or right
void tetris_move(GameState *state, int direction) {
  tetris_remove(state, &state->falling);

  state->falling.loc.col += direction;
  if (!tetris_fits(state, &state->falling)) {
    state->falling.loc.col -= direction;
  }

  tetris_put(state, &state->falling);
}

void tetris_rotate(GameState *state) {
  tetris_remove(state, &state->falling);

  state->falling.rotation++;
  state->falling.rotation %= 4;
  if (!tetris_fits(state, &state->falling)) {
    state->falling.rotation--;
    state->falling.rotation %= 4;
  }

  tetris_put(state, &state->falling);
}

void tetris_soft_drop(GameState *state) {
  tetris_remove(state, &state->falling);

  state->falling.loc.row++;
  if (!tetris_fits(state, &state->falling)) {
    state->falling.loc.row--;
  }

  tetris_put(state, &state->falling);
   
  return;
}
void tetris_hard_drop(GameState *state) {
  tetris_remove(state, &state->falling);

  while (tetris_fits(state, &state->falling)) {
    state->falling.loc.row++;
  }

  state->falling.loc.row--;

  tetris_put(state, &state->falling);

  return;
}


// sets up game settings
void tetris_init(GameState *state) {
  // for tetromino generation
  srand(time(NULL));

  // the "->" is short for deferencing, then accessing variable | (*state).score
  state->score = 0;
  state->level = 1;
  state->points = 0;
  state->game_over = false;
  state->lines_remaining = 100;
  state->ticks_until_gravity = gravity_levels[state->level-1];

  // setting up board
  int i, j;
  for (i = 0; i < ROWS; ++i) {
    for (j = 0; j < COLS; ++j) {
      state->board[i][j] = 0;
    }
  }

  // initializing first block 
  state->next.type = 3;
  state->next.rotation = 0;
  state->next.loc.row = 0;
  state->next.loc.col = COLS / 2;

  tetris_new_falling(state);

  tetris_put(state, &state->falling);

}
