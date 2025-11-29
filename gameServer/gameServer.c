#include "gameServer.h"

// TODO
// 1. zestawianie pojedynków
// 2. uzyskanie informacji o wylosowanych mapach
// 3. wyznaczenie zaczynającego gracza
// 4. czekaj na koordynaty celu przeciwnika
// 5. sprawdź czy trafiono
// 6. jeżeli tak -> wyślij informacje o tym do obu, następnie wyślij dla atakującego YOUR_TURN , goto 4
// 7. jeżeli nie -> zamień graczy, goto 4

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

    while (true)
    {
        clientNumber = dhcp_server_get_client_number();
        dhcpInfo = dhcp_server_get_client_info();

        printf("\n--- DHCP Server Scan: %u aktywnych klientow ---\n\r", clientNumber);

        if (clientNumber == 0)
        {
            printf("Brak aktywnych klientow.\n\r");
        }
        else
        {
            for (uint32_t i = 0; i < clientNumber; ++i)
            {
                printf("Klient %lu / Host ID: %u\n\r", i + 1, dhcpInfo[i].hostID);

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

static void gameTestConnection(void)
{
    const dhcp_client_info_t *dhcpInfo = NULL;
    uint32_t clientNumber = 0;

    while (true)
    {
        clientNumber = dhcp_server_get_client_number();
        dhcpInfo = dhcp_server_get_client_info();

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