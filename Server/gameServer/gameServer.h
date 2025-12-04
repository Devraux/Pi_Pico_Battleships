#ifndef _GAME_SERVER_
#define _GAME_SERVER_
#include "wifi.h"
#include <stdio.h>
#include "utility.h"

#define X_COORDINATE_READ_OFFSET 1
#define Y_COORDINATE_READ_OFFSET 2


typedef enum playStatusInfo
{
    GAME_STARTED = 0,
    WAIT_FOR_FIRST_PLAYER_MOVE,
    WAIT_FOR_SECOND_PLAYER_MOVE,
    RECEIVED_COORDINATES_FROM_FIRST_PLAYER,
    RECEIVED_COORDINATES_FROM_SECOND_PLAYER,
    WAIT_FOR_FIRST_PLAYER_MAP_INFO,
    WAIT_FOR_SECOND_PLAYER_MAP_INFO,
    GET_FIRST_PLAYER_MAP,
    GET_SECOND_PLAYER_MAP,
    NEXT_MOVE_FIRST_PLAYER,
    NEXT_MOVE_SECOND_PLAYER,
    GAME_FINISHED,
    WAIT_FOR_FIRST_PLAYER_REMACH_INFO,
    WAIT_FOR_SECOND_PLAYER_REMACH_INFO,
    GET_FIRST_PLAYER_REMACH_INFO,
    GET_SECOND_PLAYER_REMACH_INFO,
    GAME_IDLE,
    MATCH_KILL // permanent finish game, no rematch option

} playStatusInfo;

typedef struct matchStatus
{
    // CURRENT PLAY STATE
    playStatusInfo playStatus;

    // PLAY NUMBER
    uint8_t playNumber;

    // PLAYER ID
    uint8_t firstPlayer;  // last octet of player IP address
    uint8_t secondPlayer; // last octet of player IP address

    // PLAYER REMAINING HITS -> EVERY PLAYER START WITH 17
    uint8_t firstPlayerRemainingHits;
    uint8_t secondPlayerRemainingHits;

    // PLAYER  BATTLE SHIP MAP'S
    uint8_t firstPlayerShipMap[BOARD_SIZE_Y][BOARD_SIZE_X];
    uint8_t secondPlayerShipMap[BOARD_SIZE_Y][BOARD_SIZE_X];

    // RECEIVED COORDINATE FROM PLAYERS
    uint8_t coordinateX;
    uint8_t coordinateY;

    // FLAG USED TO INDICATE REMACH
    bool isGameContinued;

} matchStatus;

bool gameServerInit(void);
void gameMatchEnemies(void);
void battleShipGame(void);

#endif