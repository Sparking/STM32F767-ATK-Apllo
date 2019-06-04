#include "gpio.h"

void gpio_switch_io_mode(GPIO_TypeDef *__RESTRICT GPIO,
    const uint16_t pin, const uint8_t mode)
{
    uint8_t mode_pos;

    mode_pos = POSITION_VAL(pin) << 1;
    GPIO->MODER &= (uint32_t)~(3UL << mode_pos);
    GPIO->MODER |= (uint32_t)(mode << mode_pos);
}
