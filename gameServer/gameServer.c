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
    activeMatchNumber = (uint8_t)(clientNumber / 2);
    uint8_t secondPlayerID = 1; // start with index 1

    for (uint8_t i = 0; i < activeMatchNumber; i++)
    {

        gameStatus[i].firstPlayer = dhcpInfo[secondPlayerID - 1].AP_IP_OCTET_4;
        gameStatus[i].secondPlayer = dhcpInfo[secondPlayerID].AP_IP_OCTET_4;

        gameStatus[i].firstPlayerRemainingHists = INITIAL_SHIP_POINTS;
        gameStatus[i].secondPlayerRemainingHits = INITIAL_SHIP_POINTS;

        gameStatus[i].firstPlayerShipMap;
        gameStatus[i].secondPlayerShipMap;
        secondPlayerID += 2;
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