#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define BOARD_ROWS 20 
#define BOARD_COLS 10

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
  int board[BOARD_ROWS][BOARD_COLS];
  int score;
  int level;
  int points;
  bool gameOver;

  Piece falling; // pointer to the current falling piece struct
  Piece next;
  Piece hold;

  int linesRemaining;
  int ticksUntilGravity;

} GameState;

// function prototypes
void tetrisInitGame(GameState *state);
void tetrisMove(GameState *state, int direction); // -1 for left, +1 for right
void tetrisRotate(GameState *state);
void tetrisSoftDrop(GameState *state);
void tetrisHardDrop(GameState *state);
bool tetrisFits(GameState *state, Piece *block);
void tetrisGravity(GameState *state);
bool tetrisTick(GameState *state);  // Game loop tick, returns true if piece locked
                                     
#endif
