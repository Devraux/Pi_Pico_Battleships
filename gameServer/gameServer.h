#ifndef _GAME_SERVER_
#define _GAME_SERVER_
#include "wifi.h"
#include "dhcpServer.h"
#include <stdio.h>

#define SERVER_SID "battleShipsServer"
#define SERVER_PASSWORD "password"

bool gameServerInit(void);
void gameMatchEnemies(void);

#endif