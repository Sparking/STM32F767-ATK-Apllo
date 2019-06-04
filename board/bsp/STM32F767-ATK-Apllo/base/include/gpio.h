#ifndef __GPIO_CONFIG_H_
#define __GPIO_CONFIG_H_

#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_gpio.h>

typedef struct {
        GPIO_TypeDef *gpio;
        unsigned int  pin;
} single_gpio_pin_t;

__STATIC_FORCEINLINE void gpio_clk_enable(GPIO_TypeDef *__RESTRICT GPIO)
{
    RCC->AHB2ENR |= 1 << (((uint32_t)GPIO - GPIOA_BASE) >> 10);
}

__STATIC_FORCEINLINE void gpio_set_pin(GPIO_TypeDef *__RESTRICT GPIO, const uint16_t pin)
{
    GPIO->BSRR = (uint32_t)pin;
}

__STATIC_FORCEINLINE void gpio_reset_pin(GPIO_TypeDef *__RESTRICT GPIO, const uint16_t pin)
{
    GPIO->BSRR = (uint32_t)pin << 16;
}

extern void gpio_switch_io_mode(GPIO_TypeDef *__RESTRICT GPIO,
    const uint16_t pin, const uint8_t mode);

#endif

