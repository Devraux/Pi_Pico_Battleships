#include "battleship.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} Rect_t;



inline static void Battleship_setCursorPosition(const Board_t *board, int x, int y){
    setCursorPosition(
        (x + 1) * 2 + board->position.x,
        y + 1 + board->position.y
    );
}

inline static void Battleship_displayShipInfo(const Ship_t ship){
    printf("Ship: %i, %c ", ship.position.x, 'a' + ship.position.y);
    switch (ship.direction){
        case Up:{
            printf("Up ");
        } break;
        case Down:{
            printf("Down ");
        } break;
        case Right:{
            printf("Right ");
        } break;
        case Left:{
            printf("Left ");
        } break;
    }
    printf("%i\n", ship.size);
}

static inline bool checkRectCollision(Rect_t r1, Rect_t r2){
    return (
        (r1.x1 < r2.x2 && r1.x2 > r2.x1) &&
        (r1.y1 < r2.y2 && r1.y2 > r2.y1)
    );

}

static inline Rect_t Battleship_wrapShipRectangle(const Ship_t ship){
    Rect_t rect;
    switch (ship.direction){
        case Up:{
            rect.x1 = ship.position.x - 1;
            rect.x2 = ship.position.x + 1;
            rect.y1 = ship.position.y - 1 - ship.size;
            rect.y2 = ship.position.y + 1;
        } break;
        case Down:{
            rect.x1 = ship.position.x - 1;
            rect.x2 = ship.position.x + 1;
            rect.y1 = ship.position.y - 1;
            rect.y2 = ship.position.y + 1 + ship.size;
        } break;
        case Right:{
            rect.x1 = ship.position.x - 1;
            rect.x2 = ship.position.x + 1 + ship.size;
            rect.y1 = ship.position.y - 1;
            rect.y2 = ship.position.y + 1;
        } break;
        case Left:{
            rect.x1 = ship.position.x - 1 - ship.size;
            rect.x2 = ship.position.x + 1;
            rect.y1 = ship.position.y - 1;
            rect.y2 = ship.position.y + 1;
        } break;
    }
    return rect;
}


static inline bool Battleship_canPlaceShip(const Board_t *board, const Ship_t ship, const int index){
    switch (ship.direction){
        case Up: {
            if (ship.position.y - ship.size < 0) return false;
        } break;
        case Down: {
            if (ship.position.y + ship.size > BOARD_SIZE_Y) return false;
        } break;
        case Right: {
            if (ship.position.x + ship.size > BOARD_SIZE_X) return false;
        } break;
        case Left: {
            if (ship.position.x - ship.size < 0) return false;
        } break;
    }

    Rect_t ship_rect = Battleship_wrapShipRectangle(ship);
    for (int i = 0; i < index; i++){
        Rect_t other_rect = Battleship_wrapShipRectangle(board->ships[i]);
        if (checkRectCollision(other_rect, ship_rect))
            return false;
    }

    return true;
}

void Battleship_drawBoard(const Board_t *board){
    setCursorPosition(board->position.x, board->position.y);
    for (int y = 0; y <= BOARD_SIZE_Y; y++){
        for (int x = 0; x <= BOARD_SIZE_X; x++){
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

static inline void Battleship_drawShip(const Board_t *board, const Ship_t ship){
    for (int j = 0; j < ship.size; j++){
        switch (ship.direction){
            case Up:{
                Battleship_setCursorPosition(board, ship.position.x, ship.position.y - j);
            } break;
            case Down:{
                Battleship_setCursorPosition(board, ship.position.x, ship.position.y + j);
            } break;
            case Right:{
                Battleship_setCursorPosition(board, ship.position.x + j, ship.position.y);
            } break;
            case Left:{
                Battleship_setCursorPosition(board, ship.position.x - j, ship.position.y);
            } break;
        }


        if (ship.status == Life){
            setForegroundColor(Green);
            putchar(SHIP);
        } else {
            setForegroundColor(Red);
            putchar(DESTROY_SHIP);
        }
        fflush(stdout);
        setDefaultColor();
    }
}

void Battleship_drawShips(const Board_t *board){
    for (int i = 0; i < NUMBER_OF_SHIP; i++){
        ShipStatus_t status = board->ships[i].status;

        Battleship_drawShip(board, board->ships[i]);
        fflush(stdout);
    }
}

void Battleship_generateShipPosition(Board_t *board){
    int possibleSize[NUMBER_OF_SHIP] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int i = 0; i < NUMBER_OF_SHIP; i++){
        Ship_t ship = {
            .position.x= rand() % BOARD_SIZE_X,
            .position.y= rand() % BOARD_SIZE_Y,
            .size = possibleSize[i],
            .direction = rand() % 4,
            .status = Life
        };

        if (!Battleship_canPlaceShip(board, ship, i)){
            i--;
            continue;
        }

        board->ships[i] = ship;

        Battleship_drawShip(board, board->ships[i]);
        Battleship_setCursorPosition(board, BOARD_SIZE_X + 2, i);
        Battleship_displayShipInfo(ship);
    }
}

void Battleship_fill(const Board_t *board, char c){
    for (int y = 0; y < BOARD_SIZE_X; y++){
        setCursorPosition(1, board->position.y + 1 + y);
        for (int x = 0; x < BOARD_SIZE_Y; x++){
            printf(" %c", c);
        }
        fflush(stdout);
    }
}

ShipStatus_t Battleship_checkShip(const Board_t *board, Point_t shipPosition){
    for (int i = 0 ; i < NUMBER_OF_SHIP; i++){
        if (
            board->ships[i].position.x == shipPosition.x,
            board->ships[i].position.y == shipPosition.y
        ) return board->ships[i].status;
    }
    return None;
}

void Battleship_destroyShip(const Board_t *board, Point_t shipPosition){
    Battleship_setCursorPosition(board, shipPosition.x, shipPosition.y);

    setForegroundColor(Red);
    putchar(DESTROY_SHIP);
    setDefaultColor();
    fflush(stdout);
}