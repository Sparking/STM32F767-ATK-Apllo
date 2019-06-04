#ifndef __IIC_H_
#define __IIC_H_

#include "gpio.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define I2C_STATUS_OK       0
#define I2C_STATUS_FAILED   1

typedef struct {
    single_gpio_pin_t scl;
    single_gpio_pin_t sda;
    SemaphoreHandle_t mutex;
} i2c_bus_t;

typedef struct {
    const i2c_bus_t *bus;
    uint8_t addr;
} i2c_device_t;

__STATIC_FORCEINLINE void i2c_init(i2c_device_t *__RESTRICT i2c_dev, const i2c_bus_t *__RESTRICT bus,
    const uint8_t addr)
{
    i2c_dev->bus = bus;
    i2c_dev->addr = addr;
}

extern void i2c_bus_init(i2c_bus_t *__RESTRICT interface);
extern uint8_t i2c_write_byte(const i2c_device_t *__RESTRICT i2c_dev, const uint8_t reg, const uint8_t data);
extern uint8_t i2c_read_byte(const i2c_device_t *__RESTRICT i2c_dev, const uint8_t reg);
extern uint8_t i2c_write_bytes(const i2c_device_t *__RESTRICT i2c_dev, const uint8_t *__RESTRICT buf,
    const uint8_t len, const uint8_t reg);
extern uint8_t i2c_read_bytes(const i2c_device_t *__RESTRICT i2c_dev, uint8_t *__RESTRICT buf,
    const uint8_t len, const uint8_t reg);
extern uint8_t i2c_only_read_bytes(const i2c_device_t *__RESTRICT i2c_dev, uint8_t *__RESTRICT buf,
    const uint8_t len);

#endif /* End of __IIC_H_ */

