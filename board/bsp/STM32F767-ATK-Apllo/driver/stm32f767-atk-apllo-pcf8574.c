#include <stm32f7xx_hal.h>
#include "gpio.h"
#include "iic.h"
#include "sys.h"
#include "stm32f767-atk-apllo-pcf8574.h"

#define PCF8574_ADDR        0x20

static i2c_dev_t pcf8574;
static i2c_bus_t pcf8574_bus;

void stm32f767_atk_apllo_pcf8574_init(void)
{
    GPIO_InitTypeDef gpio;

    i2c_bus_init(&pcf8574_bus, GPIOH, GPIO_PIN_4, GPIOH, GPIO_PIN_5);
    i2c_init(&pcf8574, &pcf8574_bus, PCF8574_ADDR);
    gpio.Pin = GPIO_PIN_12;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pull = GPIO_PULLUP;
    gpio_clk_enable(GPIOB);
    HAL_GPIO_Init(GPIOB, &gpio);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 3);
    stm32f767_atk_apllo_pcf8574_read();
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

unsigned char stm32f767_atk_apllo_pcf8574_read(void)
{
    return i2c_read_byte_direct(&pcf8574);
}

void stm32f767_atk_apllo_pcf8574_write(const unsigned char data)
{
    i2c_write_bytes_direct(&pcf8574, &data, 1);
}

void stm32f767_atk_apllo_pcf8574_setbit(const unsigned char bit, const unsigned char v)
{
    unsigned char data;
    const unsigned char bit_mask = 1 << bit;

    data = stm32f767_atk_apllo_pcf8574_read();
    if (v) {
        data |= bit_mask;
    } else {
        data &= ~bit_mask;
    }

    stm32f767_atk_apllo_pcf8574_write(data);
}

unsigned char stm32f767_atk_apllo_pcf8574_getbit(const unsigned char bit)
{
    return (stm32f767_atk_apllo_pcf8574_read() & (1 << bit)) != 0;
}
