#pragma once

#include <iic.h>
#define STM32F767_ATK_APLLO_IIC_DEV_BUS_NUM     1

extern i2c_bus_t i2c_bus[STM32F767_ATK_APLLO_IIC_DEV_BUS_NUM];
i2c_dev_t i2c_at24c02;

extern void stm32f767_atk_apllo_iic_init(void);