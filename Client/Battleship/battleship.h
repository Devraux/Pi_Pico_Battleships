#ifndef __BATTLESHIP_H__
#define __BATTLESHIP_H__

#include <stdbool.h>

#define NONE_FILED              ' '
#define UNKNOWN_FIELD           '?'
#define LIFE_SHIP_FIELD         'S'
#define DESTROY_SHIP_FIELD      'X'

#define NUMBER_OF_SHIP          10
#define BOARD_SIZE_X            10
#define BOARD_SIZE_Y            10
#define SHIPS_FIELD             5

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

typedef enum {
    None,
    Unknow,
    Life,
    Dead
} ShipStatus_t;

typedef struct {
    Point_t position;
    int size;
    Direction_t direction;
} Ship_t;

typedef struct{
    Point_t position;
    ShipStatus_t playField[NUMBER_OF_SHIP][NUMBER_OF_SHIP];
    int ships_filed;
} Board_t;


void Battleship_drawBoard(const Board_t *board);
void Battleship_drawShips(const Board_t *board);
void Battleship_draw(const Board_t *Board_t);

void Battleship_generateShipPosition(Board_t *board);
void Battleship_fill(Board_t *board, ShipStatus_t status);

Point_t Battleship_readUserInput(const Board_t *board);
bool Battleship_checkShot(const Board_t *board, Point_t shot);
void Battleship_put(Board_t *board, Point_t position, ShipStatus_t status);


#endif