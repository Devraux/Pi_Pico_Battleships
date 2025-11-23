#include <stdio.h>
#include <stdlib.h>

#include "terminal.h"
#include "battleship.h"


int main(){
    Board_t board = {
        .size = {
            .x = 10,
            .y = 10
        },
        .position = {
            .x = 0,
            .y = 0,
        }
    };

    Board_t enemy = {
        .size = board.size,
        .position = {
            .x = 0,
            .y = board.size.y + board.position.y + 2
        }
    };

    clearTerminal();
    Battleship_drawBoard(&board);
    Battleship_drawBoard(&enemy);

    Battleship_generateShipPosition(&board, 10);
    Battleship_drawShips(&board);
    cursorPos(0, 24);
    free(board.ships);
    return 0;
}
