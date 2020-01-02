#pragma once

#include <stdbool.h>
#include "iic.h"

extern bool stm32f767_atk_apllo_mpu9250_init(i2c_bus_t *__RESTRICT);

extern float stm32f767_atk_apllo_mpu9250_temperature(void);

extern bool stm32f767_atk_apllo_mpu9250_gyroscope(short *__RESTRICT gx, short *__RESTRICT gy, short *__RESTRICT gz);

extern bool stm32f767_atk_apllo_mpu9250_accelerometer(short *__RESTRICT ax, short *__RESTRICT ay, short *__RESTRICT az);

extern bool stm32f767_atk_apllo_mpu9250_magnetometer(short *__RESTRICT mx, short *__RESTRICT my, short *__RESTRICT mz);
