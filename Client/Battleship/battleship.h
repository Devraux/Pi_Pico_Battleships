#ifndef __BATTLESHIP_H__
#define __BATTLESHIP_H__

#include <stdbool.h>

typedef struct {
    int x;
    int y;
} Point_t;

typedef struct {
    Point_t position;
    bool isLife;
} Ship_t;

typedef struct{
    Point_t size;
    Point_t position;

    Ship_t *ships;
    int numberOfShip;
} Board_t;


void Battleship_drawBoard(Board_t *board);
void Battleship_drawShips(Board_t *board);

void Battleship_generateShipPosition(Board_t *board, int numberOfShip);

void Battleship_putEnemy(int x, int y);


#endif