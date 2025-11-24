#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "terminal.h"
#include "battleship.h"


int main(){
    Board_t board = {
        .position = {
            .x = 0,
            .y = 0,
        }
    };

    Board_t enemy = {
        .position = {
            .x = 0,
            .y = BOARD_SIZE_X + board.position.y + 2
        }
    };

    srand(time(NULL));
    clearTerminal();
    Battleship_drawBoard(&board);
    Battleship_drawBoard(&enemy);

    Battleship_generateShipPosition(&board);
    Battleship_drawShips(&board);


    setForegroundColor(Light_black);
    Battleship_fill(&enemy, '?');


    // Battleship_destroyShip(&board, (Point_t){6, 2});
    setCursorPosition(0, 24);
    return 0;
}
