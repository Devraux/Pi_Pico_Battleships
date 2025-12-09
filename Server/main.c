#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "gameServer.h"


int main(){
    stdio_init_all();
    gameServerInit();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    battleShipGame();
    while(true){
        sleep_ms(200);
    }
}