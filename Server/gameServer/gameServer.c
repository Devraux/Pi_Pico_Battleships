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

// return playNumber based on PLAYER IP
static uint8_t getMatchNumber(const ip_addr_t *addr)
{
    const uint32_t ip_u32 = ip4_addr_get_u32(ip_2_ip4(addr));
    const uint8_t lastOctet = (uint8_t)(ip_u32 & 0xFF);

    uint8_t clientNumber = dhcpServerGetClientNumber();
    activeMatchNumber = (uint8_t)(clientNumber / 2);
    for (uint8_t i = 0; i < activeMatchNumber; i++)
    {
        if (gameStatus[i].firstPlayer == lastOctet || gameStatus[i].secondPlayer == lastOctet)
        {
            return gameStatus[i].playNumber;
        }
    }
}

static uint8_t getPlayerNumber(const ip_addr_t *addr)
{
    const uint32_t ip_u32 = ip4_addr_get_u32(ip_2_ip4(addr));
    const uint8_t lastOctet = (uint8_t)(ip_u32 & 0xFF);
    return lastOctet;
}

static void wifiReceiveCb(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    uint8_t receivedData[200] = {0};
    uint8_t matchNumber = getMatchNumber(addr);
    uint8_t playerNumber = getPlayerNumber(addr);
    if (p != NULL)
    {
        memcpy(&receivedData, p->payload, p->len);
        switch (receivedData[0]) // READ HEADER
        {
        case CLIENT_TO_SERVER_COORDINATES:
            gameStatus[matchNumber].coordinateX = receivedData[X_COORDINATE_READ_OFFSET];
            gameStatus[matchNumber].coordinateY = receivedData[Y_COORDINATE_READ_OFFSET];

            if (playerNumber == gameStatus[matchNumber].firstPlayer)
            {
                gameStatus[matchNumber].playStatus = RECEIVED_COORDINATES_FROM_FIRST_PLAYER;
            }
            else if (playerNumber == gameStatus[matchNumber].secondPlayer)
            {
                gameStatus[matchNumber].playStatus = RECEIVED_COORDINATES_FROM_SECOND_PLAYER;
            }

            break;

        case CLIENT_TO_SERVER_SHIP_MAP:
            // copy data to proper place
            if (gameStatus[matchNumber].playStatus == WAIT_FOR_FIRST_PLAYER_MAP_INFO) // received first player map
            {
                memcpy(&gameStatus[matchNumber].firstPlayerShipMap, &receivedData[1], MAP_SIZE);
                gameStatus[matchNumber].playStatus = GET_SECOND_PLAYER_MAP;
            }
            else // Second player MAP
            {
                memcpy(&gameStatus[matchNumber].secondPlayerShipMap, &receivedData[1], MAP_SIZE);
                gameStatus[matchNumber].playStatus = NEXT_MOVE_FIRST_PLAYER; // Game starts by first player move !!! <- HERE GAME STARTS !!!
            }

            break;

        case CLIENT_TO_SERVER_IS_PLAY_CONTINUED:
            if (gameStatus[matchNumber].playStatus == WAIT_FOR_FIRST_PLAYER_REMACH_INFO) // received first player map
            {
                gameStatus[matchNumber].isGameContinued &&receivedData[0];
                gameStatus[matchNumber].playStatus = GET_FIRST_PLAYER_REMACH_INFO;
            }
            else
            {
                gameStatus[matchNumber].isGameContinued &&receivedData[0];
                gameStatus[matchNumber].playStatus = GET_SECOND_PLAYER_REMACH_INFO;
            }

            break;
        }

        pbuf_free(p);
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
    while (dhcpServerGetClientNumber() == 0 || dhcpServerGetClientNumber() % 2 != 0)
    {
        printf("Game player number is odd, waiting for even number of players\n\r");
        sleep_ms(1000);
    }

    dhcpInfo = dhcpServerGetClientInfo();
    clientNumber = dhcpServerGetClientNumber();
    uint8_t secondPlayerID = 1; // start with index 1

    activeMatchNumber = (uint8_t)(clientNumber / 2);
    for (uint8_t i = 0; i < activeMatchNumber; i++)
    {
        gameStatus[i].playNumber = i;

        gameStatus[i].isGameContinued = true;

        gameStatus[i].firstPlayer = dhcpInfo[secondPlayerID - 1].AP_IP_OCTET_4;
        gameStatus[i].secondPlayer = dhcpInfo[secondPlayerID].AP_IP_OCTET_4;

        gameStatus[i].firstPlayerRemainingHits = INITIAL_SHIP_POINTS;
        gameStatus[i].secondPlayerRemainingHits = INITIAL_SHIP_POINTS;

        gameStatus[i].firstPlayerShipMap;
        gameStatus[i].secondPlayerShipMap;

        gameStatus[i].playStatus = GET_FIRST_PLAYER_MAP; // first player always begin game
        secondPlayerID += 2;
    }
}

static bool ledState = true;
void battleShipGame(void)
{
    // SERVER INITIALISATION
    gameServerInit();

    // ##############################################################################
    // ### WE HAVE TO ADD 1 MINUTE OR MORE DELAY TO LET USERS TO LOG IN TO SERVER ###
    // ##############################################################################

    // befor battleShipGame we have to use some LONG delay(60 sec or longer) to let users to connect
    gameMatchEnemies();

    // MAIN LOOP
    const dhcp_client_info_t *dhcpInfo = NULL;
    uint32_t clientNumber = 0;
    while (true)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ledState);
        ledState = !ledState;
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
            case WAIT_FOR_FIRST_PLAYER_REMACH_INFO:
            case WAIT_FOR_SECOND_PLAYER_REMACH_INFO:
            case WAIT_FOR_SECOND_PLAYER_MAP_INFO:
            case WAIT_FOR_FIRST_PLAYER_MAP_INFO:
            case GAME_IDLE:
                continue;
                break;

            case GET_FIRST_PLAYER_MAP:
                header = SERVER_TO_CLIENT_GET_SHIP_MAP;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                gameStatus[i].playStatus = WAIT_FOR_FIRST_PLAYER_MAP_INFO; // Wait for second player map info
                break;

            case GET_SECOND_PLAYER_MAP:
                header = SERVER_TO_CLIENT_GET_SHIP_MAP;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);

                break;

            case GET_FIRST_PLAYER_REMACH_INFO:
                header = SERVER_TO_CLIENT_ASK_REMATCH;
                wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);
                gameStatus[i].playStatus = WAIT_FOR_SECOND_PLAYER_REMACH_INFO;
                break;

            case GET_SECOND_PLAYER_REMACH_INFO:
                if (gameStatus[i].isGameContinued == true)
                {
                    gameStatus[i].firstPlayerRemainingHits = INITIAL_SHIP_POINTS;  // RESTORE SHIP NUMBER FOR BOTH PLAYERS
                    gameStatus[i].secondPlayerRemainingHits = INITIAL_SHIP_POINTS; // RESTORE SHIP NUMBER FOR BOTH PLAYERS
                    gameStatus[i].playStatus = GET_FIRST_PLAYER_MAP;
                }
                else
                {
                    gameStatus[i].playStatus = MATCH_KILL; // !!! PLAY FINISHED FOREVER !!! it is impossible to remach now
                }

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
                    {
                        gameStatus[i].playStatus = GAME_FINISHED;

                        header = SERVER_TO_CLIENT_WIN;
                        wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                        header = SERVER_TO_CLIENT_DEFEAT;
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
                    gameStatus[i].playStatus = NEXT_MOVE_SECOND_PLAYER;
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
                    gameStatus[i].playStatus = NEXT_MOVE_FIRST_PLAYER;
                }
                break;

            case NEXT_MOVE_FIRST_PLAYER:
                header = SERVER_TO_CLIENT_GET_COORDINATES;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                header = SERVER_TO_CLIENT_WAIT_FOR_ENEMY_MOVE;
                wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);
                gameStatus[i].playStatus = WAIT_FOR_FIRST_PLAYER_MOVE;
                break;

            case NEXT_MOVE_SECOND_PLAYER:
                header = SERVER_TO_CLIENT_GET_COORDINATES;
                wifiSendData(&header, sizeof(header), secondPlayerIp, UDP_PORT);
                header = SERVER_TO_CLIENT_WAIT_FOR_ENEMY_MOVE;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                gameStatus[i].playStatus = WAIT_FOR_SECOND_PLAYER_MOVE;
                break;

            case GAME_FINISHED:
                header = SERVER_TO_CLIENT_ASK_REMATCH;
                wifiSendData(&header, sizeof(header), firstPlayerIp, UDP_PORT);
                gameStatus[i].playStatus = WAIT_FOR_FIRST_PLAYER_REMACH_INFO;
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