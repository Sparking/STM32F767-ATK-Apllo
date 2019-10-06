#pragma once

#include "stm32f767-atk-apllo-iic-dev.h"

__STATIC_FORCEINLINE unsigned char at24c02_write_byte(const unsigned char data,
        const unsigned char addr)
{
    return i2c_write_byte(&i2c_at24c02, data, addr);
}

__STATIC_FORCEINLINE unsigned char at24c02_write_bytes(const unsigned char *__RESTRICT ptr,
        const unsigned char size, const unsigned char addr)
{
    return i2c_write_bytes(&i2c_at24c02, ptr, size, addr);
}

__STATIC_FORCEINLINE unsigned char at24c02_read_byte(const unsigned char addr)
{
    return i2c_read_byte(&i2c_at24c02, addr);
}

__STATIC_FORCEINLINE unsigned char at24c02_read_bytes(unsigned char *__RESTRICT ptr,
        const unsigned char size, const unsigned char addr)
{
    return i2c_read_bytes(&i2c_at24c02, ptr, size, addr);
}
