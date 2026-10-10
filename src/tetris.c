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
        {{0,0,0,0}, {0,0,5,0}, {5,5,5,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,5,0,0}, {0,5,0,0}, {0,5,5,0}},
        {{0,0,0,0}, {0,0,0,0}, {5,5,5,0}, {5,0,0,0}},
        {{0,0,0,0}, {5,5,0,0}, {0,5,0,0}, {0,5,0,0}}
    }
};

// gravity becomes faster for increasing levels
int gravityLevels[10] = {2, 9, 8, 7, 6, 5, 4, 3, 2, 1};

/*   HELPER FUNCTIONS FOR BLOCKS  */


static int randomTetromino(void) {
  return rand() % NUM_TETROMINOS;
}

static void tetrisNewFalling(GameState *state)
{
  // Put in a new falling tetromino.
  state->falling = state->next;
  state->next.type = randomTetromino();
  state->next.rotation = 0;
  state->next.loc.row = 0;
  state->next.loc.col = BOARD_COLS/2 - 2;
  state->ticksUntilGravity = gravityLevels[state->level-1];
}

//============================= GAME TURN FUNCTIONS ===============================

// putting the block back onto the board
void tetrisPut(GameState *state, Piece *block) {
  // right now, putting a 1x1 block
  int row = block->loc.row;
  int col = block->loc.col;

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {

        // Skip empty cells
        if (PIECES[block->type][block->rotation][i][j] == 0) continue;

        int boardY = row + i;
        int boardX = col + j;

        if (boardX >= 0 && boardX < BOARD_COLS && 
          boardY >= 0 && boardY < BOARD_ROWS) {
          state->board[boardY][boardX] = PIECES[block->type][block->rotation][i][j];
        }

      }
    }

}

// removing a block from the board
void tetrisRemove(GameState *state, Piece *block) {
  int row = block->loc.row;
  int col = block->loc.col;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {

      // Skip empty cells
      if (PIECES[block->type][block->rotation][i][j] == 0) continue;

      int boardY = row + i;
      int boardX = col + j;

      // checking bounds
      if (boardX >= 0 && boardX < BOARD_COLS && 
        boardY >= 0 && boardY < BOARD_ROWS) {
        state->board[boardY][boardX] = 0;
      }

    }
  }

}

// checking if the block fits into the current board
bool tetrisFits(GameState *state, Piece *block) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {

      // skip if it's an empty piece
      if (PIECES[block->type][block->rotation][i][j] == 0) continue;

      int boardX = block->loc.col + j;
      int boardY = block->loc.row + i;

      // checking if the piece is in bounds 
      if (boardX < 0 || boardX >= BOARD_COLS ||
        boardY < 0 || boardY >= BOARD_ROWS) {
        return false;
      }

      // checking if the place to be is a 1
      if (state->board[boardY][boardX] != 0) return false;
    }
  }

  return true;
}

void tetrisGravity(GameState *state) {
  state->ticksUntilGravity--;

  if (state->ticksUntilGravity <= 0)  {

    // removing piece to check if its new position works
    tetrisRemove(state, &state->falling);

    state->falling.loc.row++;

    // if the tetris piece "fits" (cannot move any longer), reset and create new block. 
    // true fits means it can move more, false fits mean it cannot move any longer
    if (!tetrisFits(state, &state->falling)) {
      state->falling.loc.row--;
      tetrisPut(state, &state->falling);
      tetrisNewFalling(state);

    } else {
      tetrisPut(state, &state->falling);
      state->ticksUntilGravity = gravityLevels[state->level-1];
    }

  }

}

void tetrisCheckLines(GameState *state) {
  for (int i = 0; i < BOARD_ROWS; ++i) {
    bool full = true;
    for (int j = 0; j < BOARD_COLS; ++j) {
      if (state->board[i][j] == 0) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; --k) {
        for (int l = 0; l < BOARD_COLS; ++l) {
          state->board[k][l] = state->board[k-1][l];
        }
      }
      i++;
    }
  }

  return;
}

bool tetrisTick(GameState *state) {
  tetrisGravity(state);
  tetrisCheckLines(state);

  return true;
}

//======================= MOVEMENT FUNCTIONS ===============================//

// make the current piece move left or right
void tetrisMove(GameState *state, int direction) {
  tetrisRemove(state, &state->falling);

  state->falling.loc.col += direction;
  if (!tetrisFits(state, &state->falling)) {
    state->falling.loc.col -= direction;
  }

  tetrisPut(state, &state->falling);
}

void tetrisRotate(GameState *state) {
  tetrisRemove(state, &state->falling);

  state->falling.rotation++;
  state->falling.rotation %= 4;
  if (!tetrisFits(state, &state->falling)) {
    state->falling.rotation--;
    state->falling.rotation %= 4;
  }

  tetrisPut(state, &state->falling);
}

void tetrisSoftDrop(GameState *state) {
  tetrisRemove(state, &state->falling);

  state->falling.loc.row++;
  if (!tetrisFits(state, &state->falling)) {
    state->falling.loc.row--;
  }

  tetrisPut(state, &state->falling);
   
  return;
}
void tetrisHardDrop(GameState *state) {
  tetrisRemove(state, &state->falling);

  while (tetrisFits(state, &state->falling)) {
    state->falling.loc.row++;
  }

  state->falling.loc.row--;

  tetrisPut(state, &state->falling);

  return;
}


// sets up game settings
void tetrisInitGame(GameState *state) {
  // for tetromino generation
  srand(time(NULL));

  // the "->" is short for deferencing, then accessing variable | (*state).score
  state->score = 0;
  state->level = 1;
  state->points = 0;
  state->gameOver = false;
  state->linesRemaining = 100;
  state->ticksUntilGravity = gravityLevels[state->level-1];

  // setting up board
  int i, j;
  for (i = 0; i < BOARD_ROWS; ++i) {
    for (j = 0; j < BOARD_COLS; ++j) {
      state->board[i][j] = 0;
    }
  }

  // initializing first block 
  state->next.type = 3;
  state->next.rotation = 0;
  state->next.loc.row = 0;
  state->next.loc.col = BOARD_COLS / 2;

  tetrisNewFalling(state);

  tetrisPut(state, &state->falling);

}
