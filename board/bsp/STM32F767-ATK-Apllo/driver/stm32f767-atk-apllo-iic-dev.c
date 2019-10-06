#include <iic.h>
#include "stm32f767-atk-apllo-iic-dev.h"

i2c_bus_t i2c_bus[STM32F767_ATK_APLLO_IIC_DEV_BUS_NUM];
i2c_dev_t i2c_at24c02;

void stm32f767_atk_apllo_iic_init(void)
{
    i2c_bus_init(&i2c_bus[0], GPIOH, GPIO_PIN_4, GPIOH, GPIO_PIN_5);
    i2c_init(&i2c_at24c02, &i2c_bus[0], 0xA0 >> 1);
}