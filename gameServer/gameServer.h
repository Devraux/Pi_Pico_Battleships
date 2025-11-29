#ifndef _GAME_SERVER_
#define _GAME_SERVER_
#include "wifi.h"
#include <stdio.h>

#define SERVER_SID "battleShipsServer"
#define SERVER_PASSWORD "password"
#define INITIAL_SHIP_POINTS 17

typedef enum nextMove
{
    firstPlayer = 1,
    secondPlayer = 2
} nextMove;

typedef struct matchStatus
{
    // PLAYER ID
    uint8_t firstPlayer;  // last octet of player IP address
    uint8_t secondPlayer; // last octet of player IP address

    // PLAYER REMAINING HITS -> EVERY PLAYER START WITH 17
    uint8_t firstPlayerRemainingHists;
    uint8_t secondPlayerRemainingHits;

    // PLAYER  BATTLE SHIP MAP'S
    uint8_t firstPlayerShipMap[10][10];
    uint8_t secondPlayerShipMap[10][10];
} matchStatus;

bool gameServerInit(void);
void gameMatchEnemies(void);

#endif