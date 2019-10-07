#pragma once

#include "stm32f767-atk-apllo-iic.h"

/**
 * @brief at24c02_write_byte 向at24c02写入一字节的数据
 * @param data 数据
 * @param addr 存储地址
 * @returns 写入成功返回I2C_STATUS_OK, 否则返回I2C_STATUS_FAILED
 */
__STATIC_FORCEINLINE unsigned char at24c02_write_byte(const unsigned char data,
        const unsigned char addr)
{
    return i2c_write_byte(&i2c_at24c02, data, addr);
}

/**
 * @brief at24c02_write_byte 向at24c02写入多字节的数据
 * @param ptr 数据地址
 * @param size 数据的字节数
 * @param addr 存储地址
 * @returns 写入成功返回I2C_STATUS_OK, 否则返回I2C_STATUS_FAILED
 */
__STATIC_FORCEINLINE unsigned char at24c02_write_bytes(const unsigned char *__RESTRICT ptr,
        const unsigned char size, const unsigned char addr)
{
    return i2c_write_bytes(&i2c_at24c02, ptr, size, addr);
}

/**
 * @brief at24c02_read_byte 从at24c02设备中读取一字节的数据
 * @param addr 存储地址
 * @returns 返回读出的数据
 */
__STATIC_FORCEINLINE unsigned char at24c02_read_byte(const unsigned char addr)
{
    return i2c_read_byte(&i2c_at24c02, addr);
}

/**
 * @brief at24c02_read_bytes 从at24c02中读取多字节的数据
 * @param ptr 数据地址
 * @param size 数据的大小
 * @param addr 存储地址
 * @returns 读取成功返回I2C_STATUS_OK, 否则返回I2C_STATUS_FAILED
 */
__STATIC_FORCEINLINE unsigned char at24c02_read_bytes(unsigned char *__RESTRICT ptr,
        const unsigned char size, const unsigned char addr)
{
    return i2c_read_bytes(&i2c_at24c02, ptr, size, addr);
}
