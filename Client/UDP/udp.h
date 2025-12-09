#ifndef __UDP_H__
#define __UDP_H__

#include <pico/multicore.h>
#include "lwip/err.h"

#define UDP_SEND_QUEUE_SIZE 100
#define UDP_RECV_QUEUE_SIZE 20

err_t UDP_init();

// Get raw ip address in byteorder
// @returns uint32
uint32_t get_IP();

// Convert ip address to readable format
// @param byteorder_IP ip in byteorder
// @param out return buffer (minimum 16)
int convert_IP(uint32_t byteorder_IP, char *out);

err_t UDP_write(const uint16_t port, const char *msg, const size_t len);
err_t UDP_printf(const uint16_t port, const char *format, ...);

bool UDP_read(const char *msg);

#endif