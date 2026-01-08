#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define ROWS 20 
#define COLS 10

// amount of tetrominos
#define NUM_TETROMINOS 7 
#define NUM_ORIENTATIONS 4

// defining the location of the piece
typedef struct {
  int row;
  int col;
} PieceLocation;

typedef struct {
  PieceLocation loc;
  int rotation;
  int type;
} Piece;

// central gamestate
typedef struct {
  int board[ROWS][COLS];
  int score;
  int level;
  int points;
  bool game_over;

  Piece falling; // pointer to the current falling piece struct
  Piece next;
  Piece hold;

  int lines_remaining;
  int ticks_until_gravity;

} GameState;

// function prototypes
void tetris_init(GameState *state);
void tetris_move(GameState *state, int direction); // -1 for left, +1 for right
void tetris_rotate(GameState *state);
void tetris_soft_drop(GameState *state);
void tetris_hard_drop(GameState *state);
bool tetris_fits(GameState *state, Piece *block);
void tetris_gravity(GameState *state);
bool tetris_tick(GameState *state);  // Game loop tick, returns true if piece locked
                                     
#endif
