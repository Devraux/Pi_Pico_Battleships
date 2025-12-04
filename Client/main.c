#include <pico/stdio.h>
#include <pico/stdlib.h>

// #include <unistd.h>
// #define sleep_ms(ms) usleep(ms * 1000)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "terminal.h"
#include "battleship.h"


int main(){
    stdio_init_all();

    Board_t board = {
        .position = {
            .x = 0,
            .y = 0,
        },
        .ships_filed = INITIAL_LIFE_SHIP,
    };

    Board_t enemy = {
        .position = {
            .x = 0,
            .y = BOARD_SIZE_X + board.position.y + 2
        },
        .ships_filed = INITIAL_LIFE_SHIP,
    };

    // srand(time_us_32());
    srand(time(NULL));
    bool run = true;
    while (1){
        clearTerminal();
        Battleship_drawBoard(&board);
        Battleship_drawBoard(&enemy);

        Battleship_generateShipPosition(&board);
        Battleship_draw(&board);


        setForegroundColor(Light_black);
        Battleship_fill(&enemy, UNKNOW);
        Battleship_draw(&enemy);

        while (run){
            Point_t shot = Battleship_readUserInput(&enemy);
            bool hit = Battleship_checkShot(&board, shot);
            if (hit) {
                Battleship_put(&enemy, shot, DEAD);
                enemy.ships_filed--;
                if (enemy.ships_filed <= 0){
                    run = false;
                }
            } else {
                Battleship_put(&enemy, shot, NONE);
            }
        }




        setCursorPosition(0, 24);
        board.ships_filed = INITIAL_LIFE_SHIP,
        enemy.ships_filed = INITIAL_LIFE_SHIP,
        run = true;
        sleep_ms(250);
    }
    return 0;
}
