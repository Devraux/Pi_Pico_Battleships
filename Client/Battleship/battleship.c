#include "battleship.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>


void Battleship_drawBoard(Board_t *board){
    cursorPos(board->position.x, board->position.y);
    for (int y = 0; y <= board->size.y; y++){
        for (int x = 0; x <= board->size.x; x++){
            if (x == 0 && y ==0) {
                printf(" ");
                continue;
            } else if (x == 0) {
                printf("%c", 'a'+y-1);
            } else if (y == 0) {
                printf("% 2i", (x-1)%10);
            }
        }
        printf("\n");
    }
}


void Battleship_putEnemy(int x, int y){
    cursorPos(
      x, y
    );
    printf("o\n");
}


void Battleship_drawShips(Board_t *board){
    // cursorPos(board->position.x, board->position.y);
    // printf
    for (int i = 0; i < board->numberOfShip; i++){
        cursorPos(
            (board->ships[i].position.x + 1) * 2 + board->position.x,
            board->ships[i].position.y + 1 + board->position.y
        );

       if (board->ships[i].isLife){
           printf("o\n");
       } else {
           printf("x\n");
       }
    }
}

void Battleship_generateShipPosition(Board_t *board, int numberOfShip){
    if (board->ships != NULL){
        free(board->ships);
    }
    board->ships = (Ship_t*)(malloc(numberOfShip * sizeof(Ship_t)));
    board->numberOfShip = numberOfShip;

    for (int i = 0; i < numberOfShip; i++){
        Ship_t ship = {
            .position.x= rand() % board->size.x,
            .position.y= rand() % board->size.y,
            .isLife = true
        };

        board->ships[i] = ship;
        // printf("Ship: %i, %c\n", ship.position.x, 'a' + ship.position.y);
    }
}