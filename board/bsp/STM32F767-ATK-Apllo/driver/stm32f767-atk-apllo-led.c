#include "stm32f767-atk-apllo-led.h"

void stm32f767_atk_apllo_led_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Speed = GPIO_SPEED_FAST;
    gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_clk_enable(GPIOB);
    HAL_GPIO_Init(GPIOB, &gpio_init);
    HAL_GPIO_WritePin(GPIOB, gpio_init.Pin, STM32F767_ATK_APLLO_LED_OFF);
}
