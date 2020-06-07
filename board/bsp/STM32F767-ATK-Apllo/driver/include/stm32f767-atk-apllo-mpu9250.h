#ifndef __STM32F767_ATK_APLLO_MPU9250_H__
#define __STM32F767_ATK_APLLO_MPU9250_H__

#include <stdbool.h>
#include "iic.h"
#include "maths.h"

extern bool stm32f767_atk_apllo_mpu9250_init(const i2c_bus_t *restrict bus);

extern float stm32f767_atk_apllo_mpu9250_temperature(void);

extern bool stm32f767_atk_apllo_mpu9250_gyroscope(fvector3d_t *restrict gyro,
    hvector3d_t *restrict raw);

extern bool stm32f767_atk_apllo_mpu9250_accelerometer(fvector3d_t *restrict acc,
    hvector3d_t *restrict raw);

extern bool stm32f767_atk_apllo_mpu9250_magnetometer(fvector3d_t *restrict mag,
    hvector3d_t *restrict raw);

#endif /* __STM32F767_ATK_APLLO_MPU9250_H__ */
