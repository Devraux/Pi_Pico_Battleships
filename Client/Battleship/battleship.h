#ifndef __BATTLESHIP_H__
#define __BATTLESHIP_H__

#include <stdbool.h>


#define SHIP            'S'
#define DESTROY_SHIP    'X'

#define NUMBER_OF_SHIP 10
#define BOARD_SIZE_X 10
#define BOARD_SIZE_Y 10

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
    Life,
    Dead
} ShipStatus_t;

typedef struct {
    Point_t position;
    int size;
    Direction_t direction;
    ShipStatus_t status;
} Ship_t;

typedef struct{
    Point_t position;
    Ship_t ships[NUMBER_OF_SHIP];
} Board_t;


void Battleship_drawBoard(const Board_t *board);
void Battleship_drawShips(const Board_t *board);

void Battleship_generateShipPosition(Board_t *board);

void Battleship_fill(const Board_t *board, char c);

void Battleship_destroyShip(const Board_t *board, Point_t shipPosition);

// void Battleship_putEnemy(int x, int y);


#endif