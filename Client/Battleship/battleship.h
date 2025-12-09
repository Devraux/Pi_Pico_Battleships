#ifndef __BATTLESHIP_H__
#define __BATTLESHIP_H__

#include <stdbool.h>
#include <pico/error.h>

#include "utility.h"

#define NONE_FILED              ' '
#define UNKNOWN_FIELD           '?'
#define LIFE_SHIP_FIELD         'S'
#define DESTROY_SHIP_FIELD      'X'
#define EMPTY_FIELD             'O'

#define TURN_MSG_POS_X          26
#define TURN_MSG_POS_Y          12


typedef enum {
    Up,
    Right,
    Down,
    Left
} Direction_t;

typedef struct {
    int x;
    int y;
} Point_t;

typedef struct {
    Point_t position;
    int size;
    Direction_t direction;
} Ship_t;

typedef struct{
    Point_t position;
    ShipStatus_t playField[BOARD_SIZE_Y][BOARD_SIZE_X];
    int ships_score;
} Board_t;

void Battleship_gameLoop();

void Battleship_drawBoard(const Board_t *board);
void Battleship_drawShips(const Board_t *board);
void Battleship_draw(const Board_t *Board_t);
void Battleship_redraw(const Board_t *local, const Board_t* enemy);

void Battleship_generateShipPosition(Board_t *board);
void Battleship_fill(Board_t *board, ShipStatus_t status);

Point_t Battleship_readUserInput(const Board_t *board);
bool Battleship_checkShot(const Board_t *board, Point_t shot);
void Battleship_put(Board_t *board, Point_t position, ShipStatus_t status);

TransferPacket_t Battleship_getPacket();
enum pico_error_codes Battleship_sendMap(Board_t *board);
void Battleship_textTurnPosition(bool myTurn);
void Battleship_youWin();
void Battleship_youLose();

#endif