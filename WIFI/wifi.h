#ifndef _WIFI_
#define _WIFI_

#include <pico/stdlib.h>
#include "lwip/netif.h"

#define UDP_port 4444

// DEFAULT IP ADDRESS
#define WIFI_DEFAULT_AP_IP_O1 192 // OCTET 1
#define WIFI_DEFAULT_AP_IP_O2 168 // OCTET 2
#define WIFI_DEFAULT_AP_IP_O3 4   // OCTET 3
#define WIFI_DEFAULT_AP_IP_O4 1   // OCTET 4

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

} wifiStatus;

/// @brief Change default device IP, default is: IP: 192.168.4.1
/// @param ipO1 New IP Octet value
/// @param ipO2 New IP Octet value
/// @param ipO3 New IP Octet value
/// @param ipO4 New IP Octet value
/// @warning This func. have to be call BEFORE wifiInit
void wifiSetIp(uint8_t ipO1, uint8_t ipO2, uint8_t ipO3, uint8_t ipO4);

/// @brief Initialize WIFI Standard in mode
/// @param --
/// @return wifiStatus
wifiStatus wifiStaModeInit(void);

/// @brief Initialize WIFI as Access Point
/// @param --
/// @return wifiStatus
wifiStatus wifiApModeInit(void);

bool wifiDhcpInit(void);

#endif