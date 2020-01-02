#include <iic.h>
#include "stm32f767-atk-apllo-iic.h"
#include "stm32f767-atk-apllo-mpu9250.h"

i2c_dev_t i2c_at24c02;

void stm32f767_atk_apllo_iic_init(void)
{
	static i2c_bus_t i2c_bus;

    i2c_bus_init(&i2c_bus, GPIOH, GPIO_PIN_4, GPIOH, GPIO_PIN_5);
    i2c_init(&i2c_at24c02, &i2c_bus, 0xA0 >> 1);
    if (!stm32f767_atk_apllo_mpu9250_init(&i2c_bus))
    	printf("error: failed to initial mpu9250\r\n");
}
