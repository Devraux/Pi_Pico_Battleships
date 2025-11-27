#include "wifi.h"
#include "dhcpServer.h"

static wifiIpConfig wifiIp = {
    .AP_IP_OCTET_1 = WIFI_DEFAULT_AP_IP_O1,
    .AP_IP_OCTET_2 = WIFI_DEFAULT_AP_IP_O2,
    .AP_IP_OCTET_3 = WIFI_DEFAULT_AP_IP_O3,
    .AP_IP_OCTET_4 = WIFI_DEFAULT_AP_IP_O4};

static dhcp_server_t dhcp_server;

void wifiSetIp(uint8_t ipO1, uint8_t ipO2, uint8_t ipO3, uint8_t ipO4)
{
    wifiIp.AP_IP_OCTET_1 = ipO1;
    wifiIp.AP_IP_OCTET_2 = ipO2;
    wifiIp.AP_IP_OCTET_3 = ipO3;
    wifiIp.AP_IP_OCTET_4 = ipO4;
}

bool wifiDhcpInit(void)
{
    ip4_addr_t ip, netmask;
    IP4_ADDR(&ip, wifiIp.AP_IP_OCTET_1, wifiIp.AP_IP_OCTET_2, wifiIp.AP_IP_OCTET_3, wifiIp.AP_IP_OCTET_4);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    netif_set_addr(netif_default, &ip, &netmask, &ip);
    dhcp_server_init(&dhcp_server, &ip, &netmask);
}

wifiStatus wifiStaModeInit(void)
{
}

wifiStatus wifiApModeInit(void);
