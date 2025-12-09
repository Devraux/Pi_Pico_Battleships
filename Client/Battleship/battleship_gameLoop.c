#include "battleship.h"

#include "udp.h"
#include "terminal.h"

void Battleship_gameLoop(){
    Board_t local_board = {
        .position = {
            .x = 0,
            .y = 0,
        },
        .ships_score = INITIAL_LIFE_SHIP,
    };

    Board_t enemy_board = {
        .position = {
            .x = 0,
            .y = BOARD_SIZE_Y + local_board.position.y + 2
        },
        .ships_score = INITIAL_LIFE_SHIP,
    };


    Battleship_generateShipPosition(&local_board);
    Battleship_fill(&enemy_board, UNKNOW);
    Battleship_redraw(&local_board, &enemy_board);


    bool run = true;


    while (run){
        TransferPacket_t packet = Battleship_getPacket();
        setCursorPosition(TURN_MSG_POS_X, TURN_MSG_POS_Y+1);
        printf("Header type: %i", packet.header);
        switch (packet.header){
            case SERVER_TO_CLIENT_GET_SHIP_MAP: {
                Battleship_sendMap(&local_board);
            } break;
            case SERVER_TO_CLIENT_GET_COORDINATES: {
                Battleship_textTurnPosition(true);
                Point_t shot = Battleship_readUserInput(&enemy_board);
                bool hit = Battleship_checkShot(&enemy_board, shot);
                if (hit) {
                    Battleship_put(&enemy_board, shot, DEAD);
                    enemy_board.ships_score--;
                    if (enemy_board.ships_score <= 0){
                        run = false;
                    }
                } else {
                    Battleship_put(&enemy_board, shot, EMPTY);
                }
                Battleship_textTurnPosition(false);
            } break;

            case SERVER_TO_CLIENT_SHIP_HIT: {
                Point_t hit = {
                    .x  = packet.x,
                    .y  = packet.y
                };
                Battleship_put(&local_board, hit, DEAD);
                local_board.ships_score--;
                if (local_board.ships_score <= 0){
                    run = false;
                }
            } break;
            case SERVER_TO_CLIENT_SHIP_MISS: {
                Point_t hit = {
                    .x  = packet.x,
                    .y  = packet.y
                };
                Battleship_put(&local_board, hit, EMPTY);
            } break;
            default:{
            } break;
        }
    }

    if (local_board.ships_score > 0){
        Battleship_youWin();
    } else {
        Battleship_youLose();
    }
    setDefaultColor();
    setCursorPosition(0, 22);
}