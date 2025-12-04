#include "battleship.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

#define TRY_LIMIT 2048

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} Rect_t;



static inline void Battleship_setCursorPosition(const Board_t *board, int x, int y){
    setCursorPosition(
        (x + 1) * 2 + board->position.x,
        y + 1 + board->position.y
    );
}

static inline void Battleship_displayShipInfo(const Ship_t ship){
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


static inline bool Battleship_canPlaceShip(const Ship_t *ships, const Ship_t ship, const int index){
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
        Rect_t other_rect = Battleship_wrapShipRectangle(ships[i]);
        if (checkRectCollision(other_rect, ship_rect))
            return false;
    }

    return true;
}

static bool Battleship_ask(const Point_t shot){
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
    setForegroundColor(Green);
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


        putchar(LIFE_SHIP_FIELD);
        fflush(stdout);
    }
    setDefaultColor();
    fflush(stdout);
}

static inline void Battleship_putShipOnField(Board_t *board, const Ship_t ship){
    int x, y;
    for (int j = 0; j < ship.size; j++){
        switch (ship.direction){
            case Up:{
                x = ship.position.x;
                y = ship.position.y - j;
            } break;
            case Down:{
                x = ship.position.x;
                y = ship.position.y + j;
            } break;
            case Right:{
                x = ship.position.x + j;
                y = ship.position.y;
            } break;
            case Left:{
                x = ship.position.x - j;
                y = ship.position.y;
            } break;
        }

        board->playField[y][x] = LIFE;
    }
}

void Battleship_draw(const Board_t *board){
    for (int y = 0; y < NUMBER_OF_SHIP; y++){
        Battleship_setCursorPosition(board, 0, y);
        for (int x = 0; x < NUMBER_OF_SHIP; x++){
            switch (board->playField[y][x]){
                case NONE:
                    putchar(NONE_FILED); break;
                case UNKNOW:
                    putchar(UNKNOWN_FIELD); break;
                case LIFE:{
                    setForegroundColor(Green);
                    putchar(LIFE_SHIP_FIELD);
                } break;
                case DEAD:{
                    setForegroundColor(Red);
                    putchar(DESTROY_SHIP_FIELD);
                } break;
            }
            putchar(' ');
        }
        fflush(stdout);
    }
    setDefaultColor();
    fflush(stdout);
}

void Battleship_generateShipPosition(Board_t *board){
    int possibleSize[NUMBER_OF_SHIP] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    int try = 0;
    Ship_t ships[NUMBER_OF_SHIP];
    for (int i = 0; i < NUMBER_OF_SHIP; i++){
        Ship_t ship = {
            .position.x= rand() % BOARD_SIZE_X,
            .position.y= rand() % BOARD_SIZE_Y,
            .size = possibleSize[i],
            .direction = rand() % 4,
        };

        if (!Battleship_canPlaceShip(ships, ship, i)){
            // Battleship_setCursorPosition(board, BOARD_SIZE_X + 2, i);
            // printf("Try: %i, pos: %i, %i\n", try, ship.position.x, ship.position.y);
            try++;
            i--;
            if (try > TRY_LIMIT){
                i = 0;
            }
            continue;
        }

        ships[i] = ship;
        try = 0;

        // Battleship_drawShip(board, ship);
        Battleship_setCursorPosition(board, BOARD_SIZE_X + 2, i);
        Battleship_displayShipInfo(ship);
    }

    Battleship_fill(board, NONE);
    for (int i = 0; i < NUMBER_OF_SHIP; i++){
        Battleship_putShipOnField(board, ships[i]);
    }
}

void Battleship_fill(Board_t *board, ShipStatus_t status){
    for (int y = 0; y < BOARD_SIZE_X; y++){
        for (int x = 0; x < BOARD_SIZE_Y; x++){
            board->playField[y][x] = status;
        }
    }
}

Point_t Battleship_readUserInput(const Board_t *board){

    Battleship_setCursorPosition(board, NUMBER_OF_SHIP + 1, NUMBER_OF_SHIP - 2);
    printf("Shot:   ");
    Battleship_setCursorPosition(board, NUMBER_OF_SHIP + 4, NUMBER_OF_SHIP - 2);
    fflush(stdout);

    Point_t position = {
        .x = -1,
        .y = -1
    };

    int position_index = 0;
    while (true){
        char r_pos = getchar();
        if (
            (  (r_pos >= 'a' && r_pos <= 'a' + NUMBER_OF_SHIP)
            || (r_pos >= 'A' && r_pos <= 'A' + NUMBER_OF_SHIP))
            && position_index < 2
        ){
            putchar(r_pos);
            if (r_pos <= 'Z') {
                r_pos += 'a' - 'A';
            }
            position.y = r_pos - 'a';
            position_index++;
        } else if (
            (r_pos >= '0' && r_pos <='9')
            && position_index < 2
        ){
            putchar(r_pos);
            position.x = r_pos - '0';
            position_index++;
        } else if (
            (r_pos == 8 || r_pos == 127)
            && position_index > 0
        ) {
            printf("\b \b");
            position_index--;
        } else if (r_pos == '\r' || r_pos == '\n') {
            if (position.x == -1 || position.y == -1){
                Battleship_setCursorPosition(board, NUMBER_OF_SHIP + 1, NUMBER_OF_SHIP - 2);
                printf("Shot:   ");
                Battleship_setCursorPosition(board, NUMBER_OF_SHIP + 4, NUMBER_OF_SHIP - 2);
                fflush(stdout);

                position = (Point_t){
                    .x = -1,
                    .y = -1
                };
                position_index = 0;
                continue;
            }
            break;
        }
        fflush(stdout);
    }

    Battleship_setCursorPosition(board, NUMBER_OF_SHIP + 1, NUMBER_OF_SHIP - 1);
    printf("(%i, %i)", position.x, position.y);
    fflush(stdout);

    return position;
}

bool Battleship_checkShot(const Board_t *board, const Point_t shot){
    if (board->playField[shot.y][shot.x] == UNKNOW){
        return Battleship_ask(shot);
    } else if (board->playField[shot.y][shot.x] == LIFE){
        return true;
    } else {
        return false;
    }
}

void Battleship_put(Board_t *board, Point_t position, ShipStatus_t status){
    Battleship_setCursorPosition(board, position.x, position.y);
    switch (status){
        case NONE:
            putchar(NONE_FILED); break;
        case UNKNOW:
            putchar(UNKNOWN_FIELD); break;
        case LIFE:{
            setForegroundColor(Green);
            putchar(LIFE_SHIP_FIELD);
        } break;
        case DEAD:{
            setForegroundColor(Red);
            putchar(DESTROY_SHIP_FIELD);
        } break;
    }
    setDefaultColor();
}