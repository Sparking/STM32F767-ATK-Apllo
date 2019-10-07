#pragma once

#include <gpio.h>

#define STM32F767_ATK_APLLO_LED_GREEN   0
#define STM32F767_ATK_APLLO_LED_RED     1

#define STM32F767_ATK_APLLO_LED_ON  GPIO_PIN_RESET
#define STM32F767_ATK_APLLO_LED_OFF GPIO_PIN_SET

extern void stm32f767_atk_apllo_led_init(void);

__STATIC_FORCEINLINE void stm32f767_atk_apllo_led_set(const unsigned char led, const unsigned char status)
{
    HAL_GPIO_WritePin(GPIOB, 1 << led, status);
}
