#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "gameServer.h"

#define LED_PIN 0

int main()
{

    stdio_init_all();
    gameServerInit();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);


    gameMatchEnemies();

    while (true)
    {
        printf("blinking!!!\r\n");
        cyw43_arch_gpio_put(LED_PIN, true);
        sleep_ms(200);
        cyw43_arch_gpio_put(LED_PIN, false);
        sleep_ms(200);
    }
}