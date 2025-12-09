#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "udp.h"
#include "terminal.h"
#include "battleship.h"


int main(){
    stdio_init_all();
    clearTerminal();
    if (UDP_init() != ERR_OK){
        setForegroundColor(Red);
        printf("Cannot connect to WiFi %s\n", SSID);
        setDefaultColor();
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    srand(time_us_32());
    Battleship_gameLoop();

    return 0;
}
