#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "gameServer.h"


int main()
{
    stdio_init_all();

    battleShipGame();
    while(true){
        sleep_ms(200);
    }
}