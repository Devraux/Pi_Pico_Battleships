#include "wifi.h"

static wifiIpConfig wifiIp = {
    .AP_IP_OCTET_1 = WIFI_DEFAULT_AP_IP_O1,
    .AP_IP_OCTET_2 = WIFI_DEFAULT_AP_IP_O2,
    .AP_IP_OCTET_3 = WIFI_DEFAULT_AP_IP_O3,
    .AP_IP_OCTET_4 = WIFI_DEFAULT_AP_IP_O4};

static dhcp_server_t dhcp_server;
static struct udp_pcb *receivePcb = NULL;
static struct udp_pcb *sendPcb = NULL;

static char SSID[32] = "defaultSSID";
static char PASSWORD[63] = "defaultPASSWORD";

void wifiSetIp(uint8_t ipO1, uint8_t ipO2, uint8_t ipO3, uint8_t ipO4)
{
    wifiIp.AP_IP_OCTET_1 = ipO1;
    wifiIp.AP_IP_OCTET_2 = ipO2;
    wifiIp.AP_IP_OCTET_3 = ipO3;
    wifiIp.AP_IP_OCTET_4 = ipO4;
}

wifiStatus wifiDhcpServerInit(void)
{
    ip4_addr_t ip, netmask;
    IP4_ADDR(&ip, wifiIp.AP_IP_OCTET_1, wifiIp.AP_IP_OCTET_2, wifiIp.AP_IP_OCTET_3, wifiIp.AP_IP_OCTET_4);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    netif_set_addr(netif_default, &ip, &netmask, &ip);
    dhcp_server_init(&dhcp_server, &ip, &netmask);

    return wifiDhcpInitSuccess;
}

wifiStatus wifiStaModeInit(const char *ssid, const char *password, udp_recv_fn recv, void *recv_arg)
{
    if (ssid == NULL || password == NULL)
    {
        printf("Your SSID or PASSWORD is incorrect.\n\r");
        printf("Your default SSID is: %s\n\r", SSID);
        printf("Your default PASSWORD is: %s\n\r", PASSWORD);
        printf("Default authorization is: CYW43_AUTH_OPEN. \n\r");
    }

    if (strlen(password) > WIFI_MAX_PASSWORD_LENGTH || strlen(ssid) > WIFI_MAX_SSID_LENGTH)
    {
        printf("Your SSID or PASSWORD is too long.\n\r");
        printf("Your default SSID is: %s\n\r", SSID);
        printf("Your default PASSWORD is: %s\n\r", PASSWORD);
        printf("Default authorization is: CYW43_AUTH_OPEN. \n\r");
    }

    else
    {
        snprintf(SSID, sizeof(SSID), "%s", ssid);
        snprintf(PASSWORD, sizeof(PASSWORD), "%s", password);
    }
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    int result = cyw43_arch_wifi_connect_timeout_ms(SSID, PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);

    if (result != 0)
    {
        printf("WiFi connection failed!\n");
        return wifiConfigFail;
    }

    if (receivePcb == NULL || sendPcb == NULL)
    {
        receivePcb = udp_new();
        sendPcb = udp_new();
        if (receivePcb == NULL || sendPcb == NULL)
        {
            printf("Failed to create UDP PCB\n");
            return wifiCratePcbFail;
        }

        err_t err = udp_bind(receivePcb, IP_ADDR_ANY, UDP_PORT);
        if (err == ERR_OK)
        {
            udp_recv(receivePcb, recv, recv_arg);
            return wifiConfigSuccess;
        }
        else
        {
            printf("Failed to bind UDP PCB\n");
            return wifiBindFail;
        }
    }
    else
    {
        printf("Your wifi is already configured.\n");
        return wifiConfigSuccess;
    }
}

wifiStatus wifiApModeInit(const char *ssid, const char *password, udp_recv_fn recv, void *recv_arg)
{
    if (ssid == NULL || password == NULL)
    {
        printf("Your SSID or PASSWORD is incorrect.\n\r");
        printf("Default SSID is: %s\n\r", SSID);
        printf("Default PASSWORD is: %s\n\r", PASSWORD);
        printf("Default authorization is: CYW43_AUTH_OPEN. \n\r");
    }

    if (strlen(password) > WIFI_MAX_PASSWORD_LENGTH || strlen(ssid) > WIFI_MAX_SSID_LENGTH)
    {
        printf("Your SSID or PASSWORD is too long.\n\r");
        printf("Your default SSID is: %s\n\r", SSID);
        printf("Your default PASSWORD is: %s\n\r", PASSWORD);
        printf("Default authorization is: CYW43_AUTH_OPEN. \n\r");
    }

    else
    {
        snprintf(SSID, sizeof(SSID), "%s", ssid);
        snprintf(PASSWORD, sizeof(PASSWORD), "%s", password);
    }

    cyw43_arch_init();
    cyw43_arch_enable_ap_mode(SSID, PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    sleep_ms(1000);

    if (receivePcb == NULL || sendPcb == NULL)
    {
        receivePcb = udp_new();
        sendPcb = udp_new();

        if (receivePcb == NULL || sendPcb == NULL)
        {
            printf("Failed to create UDP PCB\n");
            return wifiCratePcbFail;
        }

        err_t err = udp_bind(receivePcb, IP_ADDR_ANY, UDP_PORT);
        if (err == ERR_OK)
        {
            udp_recv(receivePcb, recv, recv_arg);
            return wifiConfigSuccess;
        }
        else
        {
            printf("Failed to bind UDP PCB\n");
            return wifiBindFail;
        }
    }
    else
    {
        printf("Your wifi is already configured.\n");
        return wifiConfigSuccess;
    }
}

void wifiSendData(uint8_t *data, uint32_t dataLength, const ip_addr_t *destIp, uint32_t port)
{
    if (data == NULL || dataLength == 0)
        return;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, dataLength, PBUF_RAM);

    if (p != NULL)
    {
        memcpy(p->payload, data, dataLength);
        err_t err = udp_sendto(sendPcb, p, destIp, port);
        if (err != ERR_OK)
            printf("Failed to send UDP packet\n");
        pbuf_free(p);
    }
    else
        return;
}

const dhcp_client_info_t *dhcpServerGetClientInfo(void)
{
    const dhcp_client_info_t *info = dhcp_server_get_client_info();
    return info;
}

uint32_t dhcpServerGetClientNumber(void)
{
    uint32_t clientNumber = dhcp_server_get_client_number();
    return clientNumber;
}

// const ip_addr_t *createIpAddrPtr(uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4)
// {
//     static ip_addr_t destAddr;
//     IP_ADDR4(&destAddr, octet1, octet2, octet3, octet4);

//     return &destAddr;
// }