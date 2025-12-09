#ifndef __STRUCTS_H__
#define __STRUCTS_H__
#include <pico/stdio.h>

typedef struct __attribute__((packed)) {
    uint8_t header;
    int8_t x;
    int8_t y;
} TransferPacket_t;


#endif