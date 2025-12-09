#include "udp.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <pico/util/queue.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"


#include "defines.h"
#include "PASS.h"


static struct udp_pcb*  pcb;
static ip_addr_t        server_ip;

// static queue_t send_queue;
static queue_t recv_queue;

uint32_t get_IP(){
    extern cyw43_t cyw43_state;
    return cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
}

int convert_IP(uint32_t byteorder_IP, char *out){
    return sprintf(out, "%lu.%lu.%lu.%lu", byteorder_IP & 0xFF, (byteorder_IP >> 8) & 0xFF, (byteorder_IP >> 16) & 0xFF, byteorder_IP >> 24);
}

err_t UDP_write(const uint16_t port, const char *msg, const size_t len){
    if (len >= UDP_MSG_LEN_MAX) {
        return PICO_ERROR_BUFFER_TOO_SMALL;
    }
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    err_t er = pbuf_take(p, msg, len);
    if (er == ERR_OK){
        udp_sendto(pcb, p, &server_ip, port);
    }
    pbuf_free(p);

    return er;
}


static void UDP_recvCallback(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port) {
    LWIP_UNUSED_ARG(arg);

    if (p == NULL){
        return;
    }
    char msg[UDP_MSG_LEN_MAX] = {0};
    memcpy(msg, (char *)p->payload, p->len);
    queue_try_add(&recv_queue, msg);
    // queue_try_add(&send_queue, msg);

    pbuf_free(p);
}

static void UDP_initQueue(){
    // queue_init(&send_queue, UDP_MSG_LEN_MAX * sizeof(char), UDP_SEND_QUEUE_SIZE);
    queue_init(&recv_queue, UDP_MSG_LEN_MAX * sizeof(char), UDP_RECV_QUEUE_SIZE);
}

err_t UDP_init(){
    cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND);
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK, 50000)) {
        cyw43_arch_deinit();
        printf("failed to connect.\n");
        return PICO_ERROR_TIMEOUT;
    }
    printf("Connected\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    UDP_initQueue();
    pcb = udp_new();

    char ip[16] = {0};
    ip_addr_t local_ip = {.addr = get_IP()};
    server_ip.addr = (local_ip.addr & ~(0xFF << 8*3)) | (0x01 << 8*3);

    convert_IP(server_ip.addr, ip);
    printf("Server IP: %s\n", ip);

    convert_IP(local_ip.addr, ip);
    printf("IP Address: %s\n", ip);

    udp_bind(pcb, &local_ip, SERVER_UDP_PORT);
    udp_recv(pcb, (udp_recv_fn)UDP_recvCallback, pcb);

    return PICO_ERROR_NONE;
}


err_t UDP_printf(const uint16_t port, const char *format, ...){
    char msg[UDP_MSG_LEN_MAX] = {0};

    va_list args;
    va_start(args, format);
    vsnprintf(msg, UDP_MSG_LEN_MAX-1, format, args);
    va_end(args);

    return UDP_write(port, msg, strlen(msg));
}


bool UDP_read(const char *msg){
    return queue_try_remove(&recv_queue, (void *)msg);
}