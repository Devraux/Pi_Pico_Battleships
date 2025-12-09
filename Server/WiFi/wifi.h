#ifndef _WIFI_
#define _WIFI_

#include <pico/stdlib.h>
#include <string.h>
#include "lwip/netif.h"
#include "lwip/udp.h"
#include <pico/cyw43_arch.h>
#include "dhcpServer.h"
#include "lwip/dhcp.h"

#include "defines.h"


// DEFAULT IP ADDRESS
#define WIFI_DEFAULT_AP_IP_O1 192 // OCTET 1
#define WIFI_DEFAULT_AP_IP_O2 168 // OCTET 2
#define WIFI_DEFAULT_AP_IP_O3 4   // OCTET 3
#define WIFI_DEFAULT_AP_IP_O4 1   // OCTET 4

#define WIFI_MAX_SSID_LENGTH 32
#define WIFI_MAX_PASSWORD_LENGTH 64

typedef struct wifiIpConfig
{
    uint8_t AP_IP_OCTET_1; // WIFI_DEFAULT_AP_IP_O1,
    uint8_t AP_IP_OCTET_2; // WIFI_DEFAULT_AP_IP_O2,
    uint8_t AP_IP_OCTET_3; // WIFI_DEFAULT_AP_IP_O3,
    uint8_t AP_IP_OCTET_4; // WIFI_DEFAULT_AP_IP_O4,
} wifiIpConfig;

typedef enum wifiStatus
{
    wifiNotConfigured = 0,
    wifiConfigured = 1,
    wifiCratePcbFail = 2,
    wifiBindFail = 3,
    wifiCratePcbSuccess = 4,
    wifiDhcpInitSuccess = 5,
    wifiConfigSuccess = 6,
    wifiConfigFail = 7

} wifiStatus;

/// @brief Change default device IP, default is: IP: 192.168.4.1
/// @param ipO1 New IP Octet value
/// @param ipO2 New IP Octet value
/// @param ipO3 New IP Octet value
/// @param ipO4 New IP Octet value
/// @warning This func. have to be call BEFORE wifiInit
void wifiSetIp(uint8_t ipO1, uint8_t ipO2, uint8_t ipO3, uint8_t ipO4);

/// @brief Initialize wifi Access Point mode
/// @param ssid
/// @param password
/// @param recv - wifi receive callback
/// @param recv_arg - wifi callback arg.
/// @return struct wifiStatus
wifiStatus wifiApModeInit(const char *ssid, const char *password, udp_recv_fn recv, void *recv_arg);

/// @brief Initialize wifi Standard mode
/// @param ssid
/// @param password
/// @param recv - wifi receive callback
/// @param recv_arg - wifi callback arg.
/// @return struct wifiStatus
wifiStatus wifiStaModeInit(const char *ssid, const char *password, udp_recv_fn recv, void *recv_arg);

/// @brief Initialize WIFI as Access Point
/// @param --
/// @return wifiStatus
wifiStatus wifiDhcpServerInit(void);

void wifiSendData(uint8_t *data, uint32_t dataLength, const ip_addr_t *destIp, uint32_t port);

const dhcp_client_info_t *dhcpServerGetClientInfo(void);

uint32_t dhcpServerGetClientNumber(void);

// ########################################################## //
// ########## HOW TO USE WITH CLIENT  SOFTWARE ############## //
// ########################################################## //

// #include "pico/cyw43_arch.h"
// #include "lwip/dhcp.h"

// void test(void)
// {
// cyw43_arch_init();
// cyw43_arch_enable_sta_mode();

// cyw43_arch_wifi_connect_blocking("SSID_AP", "PASSWORD", CYW43_AUTH_WPA2_AES_PSK);

// struct netif *netif = netif_default;
// dhcp_start(netif);
// sleep_ms(1000);
// printf("My IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
//}
// ########################################################## //

#endif