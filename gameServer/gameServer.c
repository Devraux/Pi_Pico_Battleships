#include "gameServer.h"

// TODO
// 1. zestawianie pojedynków
// 2. uzyskanie informacji o wylosowanych mapach
// 3. wyznaczenie zaczynającego gracza
// 4. czekaj na koordynaty celu przeciwnika
// 5. sprawdź czy trafiono
// 6. jeżeli tak -> wyślij informacje o tym do obu, następnie wyślij dla atakującego YOUR_TURN , goto 4
// 7. jeżeli nie -> zamień graczy, goto 4

static matchStatus gameStatus[7]; // Due to maximum dhcp server clients(14) maximum player number is 7 (7 * 2 = 14 active player's)
static uint8_t activeMatchNumber = 0;

static void wifiReceiveCb(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

static void wifiReceiveCb(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL)
    {
        // memcpy(&receivedData, p->payload, sizeof(device_configStatus_t));
        //  pbuf_free(p);
    }
}

bool gameServerInit(void)
{
    uint32_t status = wifiConfigFail;
    status = wifiApModeInit(SERVER_SID, SERVER_PASSWORD, &wifiReceiveCb, NULL);
    // if (status != wifiConfigSuccess)
    //     return false;

    wifiSetIp(192, 168, 4, 1);
    status = wifiDhcpServerInit();
    // if (status != wifiConfigSuccess)
    //     return false;

    return true;
}

void gameMatchEnemies(void)
{
    const dhcp_client_info_t *dhcpInfo = NULL;
    uint32_t clientNumber = 0;

    // WAIT UNTIL COMPLETE USERS WILL CONNECT
    while (dhcpServerGetClientNumber() % 2 != 0)
    {
        printf("Game player number is odd");
        sleep_ms(1000);
    }

    dhcpInfo = dhcpServerGetClientInfo();
    clientNumber = dhcpServerGetClientNumber();
    uint8_t secondPlayerID = 1; // start with index 1

    activeMatchNumber = (uint8_t)(clientNumber / 2);
    for (uint8_t i = 0; i < activeMatchNumber; i++)
    {
        gameStatus[i].firstPlayer = dhcpInfo[secondPlayerID - 1].AP_IP_OCTET_4;
        gameStatus[i].secondPlayer = dhcpInfo[secondPlayerID].AP_IP_OCTET_4;

        gameStatus[i].firstPlayerRemainingHits = INITIAL_SHIP_POINTS;
        gameStatus[i].secondPlayerRemainingHits = INITIAL_SHIP_POINTS;

        gameStatus[i].firstPlayerShipMap;
        gameStatus[i].secondPlayerShipMap;

        gameStatus[i].playStatus = GAME_STARTED; // first player always begin game
        secondPlayerID += 2;
    }
}

void battleShipGame(void)
{
    // befor battleShipGame we have to use some long delay(60 sec or longer) to let users to connect
    gameMatchEnemies();

    // MAIN LOOP
    const dhcp_client_info_t *dhcpInfo = NULL;
    uint32_t clientNumber = 0;
    while (true)
    {
        // UPDATE USER INFO EVERY LOOP ITERATION
        dhcpInfo = dhcpServerGetClientInfo();
        clientNumber = dhcpServerGetClientNumber();

        activeMatchNumber = (uint8_t)(clientNumber / 2);
        for (uint8_t i = 0; i < activeMatchNumber; i++)
        {
            uint8_t X = gameStatus[i].coordinateX;
            uint8_t Y = gameStatus[i].coordinateY;

            ip_addr_t player1_addr;
            IP_ADDR4(&player1_addr, 192, 168, 4, gameStatus[i].firstPlayer);
            const ip_addr_t *firstPlayerIp = &player1_addr;

            ip_addr_t player2_addr;
            IP_ADDR4(&player2_addr, 192, 168, 4, gameStatus[i].secondPlayer);
            const ip_addr_t *secondPlayerIp = &player2_addr;

            uint8_t header;

            switch (gameStatus[i].playStatus)
            {
            case GAME_STARTED:
                header = SERVER_TO_CLIENT_GET_SHIP_MAP;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                gameStatus[i].playStatus = WAIT_FOR_FIRST_PLAYER_MOVE;
                break;

            case WAIT_FOR_FIRST_PLAYER_MOVE:
            case WAIT_FOR_SECOND_PLAYER_MOVE:
                continue; // waiting for player response
                break;

            case RECEIVED_COORDINATES_FROM_FIRST_PLAYER:
                if (gameStatus[i].secondPlayerShipMap[X][Y] == LIFE) // First player hit ship
                {
                    gameStatus[i].secondPlayerShipMap[X][Y] = DEAD;
                    gameStatus[i].secondPlayerRemainingHits--;
                    gameStatus[i].playStatus = NEXT_MOVE_FIRST_PLAYER; // First player continue attacking

                    if (gameStatus[i].secondPlayerRemainingHits == 0) // game finished, FIRST player WIN

                        header = SERVER_TO_CLIENT_SHIP_HIT;
                    wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);  // Inform about HIT first player
                    wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT); // Inform about HIT second player
                }
                else
                {
                    header = SERVER_TO_CLIENT_SHIP_MISS;
                    wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);  // Inform about MISS first player
                    wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT); // Inform about MISS second player
                }
                break;

            case RECEIVED_COORDINATES_FROM_SECOND_PLAYER:
                if (gameStatus[i].firstPlayerShipMap[X][Y] == LIFE) // Second player hit ship
                {
                    gameStatus[i].firstPlayerShipMap[X][Y] = DEAD;
                    gameStatus[i].firstPlayerRemainingHits--;
                    gameStatus[i].playStatus = NEXT_MOVE_SECOND_PLAYER; // First player continue attacking

                    if (gameStatus[i].firstPlayerRemainingHits == 0) // game finished, SECOND player WIN
                    {
                        gameStatus[i].playStatus = GAME_FINISHED;

                        header = SERVER_TO_CLIENT_DEFEAT;
                        wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                        header = SERVER_TO_CLIENT_WIN;
                        wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);
                    }

                    header = SERVER_TO_CLIENT_SHIP_HIT;
                    wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);  // Inform about HIT first player
                    wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT); // Inform about HIT second player
                }
                else
                {
                    header = SERVER_TO_CLIENT_SHIP_MISS;
                    wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);  // Inform about MISS first player
                    wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT); // Inform about MISS second player
                }
                break;

            case NEXT_MOVE_FIRST_PLAYER:
                header = SERVER_TO_CLIENT_GET_COORDINATES;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                header = SERVER_TO_CLIENT_WAIT_FOR_ENEMY_MOVE;
                wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);

                break;

            case NEXT_MOVE_SECOND_PLAYER:
                header = SERVER_TO_CLIENT_GET_COORDINATES;
                wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);
                header = SERVER_TO_CLIENT_WAIT_FOR_ENEMY_MOVE;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                break;

            case GAME_FINISHED:
                header = SERVER_TO_CLIENT_ASK_REMATCH;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);
                break;

            case MATCH_KILL:
                continue;
                break;
            }
        }
        sleep_ms(150);
    }
}

// FOR DEBUG ONLY
static void gameTestConnection(void)
{
    const dhcp_client_info_t *dhcpInfo = NULL;
    uint32_t clientNumber = 0;

    while (true)
    {
        clientNumber = dhcpServerGetClientNumber();
        dhcpInfo = dhcpServerGetClientInfo();

        printf("\n--- DHCP Server Scan: %u active clients ---\n\r", clientNumber);

        if (clientNumber == 0)
        {
            printf("Server doesn't have any connected client .\n\r");
        }
        else
        {
            for (uint32_t i = 0; i < clientNumber; ++i)
            {
                printf("Client %lu / Host ID: %u\n\r", i + 1, dhcpInfo[i].hostID);

                // Adres MAC
                printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\n\r",
                       dhcpInfo[i].mac[0], dhcpInfo[i].mac[1], dhcpInfo[i].mac[2],
                       dhcpInfo[i].mac[3], dhcpInfo[i].mac[4], dhcpInfo[i].mac[5]);

                // Adres IP
                printf("  IP: %u.%u.%u.%u\n\r",
                       dhcpInfo[i].AP_IP_OCTET_1, dhcpInfo[i].AP_IP_OCTET_2,
                       dhcpInfo[i].AP_IP_OCTET_3, dhcpInfo[i].AP_IP_OCTET_4);
            }
            printf("-------------------------------------------------\n\r");
        }
        sleep_ms(1000);
    }
}