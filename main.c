#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define LED_PIN 0

int main()
{
    cyw43_arch_init();
    stdio_init_all();
     

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while(true)
    {
        printf("blinking!!!\r\n");
        cyw43_arch_gpio_put(LED_PIN, true);
        sleep_ms(200);
        cyw43_arch_gpio_put(LED_PIN, false);
        sleep_ms(200);
    }
}