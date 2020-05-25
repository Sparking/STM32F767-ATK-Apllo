#ifndef __BSP_STM32F767_ATK_APLLOTK_APLLO_IIC_H__
#define __BSP_STM32F767_ATK_APLLOTK_APLLO_IIC_H__

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "gpio.h"

enum {
    I2C_STATUS_OK = 0,
    I2C_STATUS_FAILED,
};

typedef struct {
    single_gpio_pin_t scl;      /* 时钟信号 */
    single_gpio_pin_t sda;      /* 数据总线 */
    SemaphoreHandle_t mutex;    /* 总线互斥锁, 可以根据需要改成自旋锁 */
} i2c_bus_t;

typedef struct {
    const i2c_bus_t *bus;       /* IIC总线 */
    unsigned char addr;         /* 向左偏移一位后的设备地址 */
} i2c_dev_t;

/**
 * @brief i2c_init 初始化一个i2c设备
 * @param i2c_dev IIC设备对应的结构体: 初始化后的总线 + 地址
 * @param bus IIC总线
 * @param addr IIC设备的地址
 */
__STATIC_FORCEINLINE void i2c_init(i2c_dev_t *restrict i2c_dev, const i2c_bus_t *restrict bus,
        const unsigned char addr) {
    i2c_dev->bus = bus;
    i2c_dev->addr = addr << 1;
}

/**
 * @brief i2c_bus_init IIC设备总线初始化
 * @param bus IIC设备总线
 * @param scl_gpio SCL对应的GPIO
 * @param scl_pin  SCL对应的GPIO上的管脚
 * @param sda_gpio SDA对应的GPIO
 * @param sda_pin  SDA对应的GPIO上的管脚
 */
extern void i2c_bus_init(i2c_bus_t *restrict bus, GPIO_TypeDef *scl_gpio, const uint16_t scl_pin,
        GPIO_TypeDef *sda_gpio, const uint16_t sda_pin);

/**
 * @brief i2c_write_byte 向IIC设备写入一个字节的数据
 * @param i2c_dev IIC设备
 * @param reg IIC设备的寄存器
 * @param data 需要写入的1字节数据
 * @returns 返回写入的状态, 成功返回I2C_STATUS_OK, 失败返回I2C_STATUS_FAILED
 */
extern unsigned char i2c_write_byte(const i2c_dev_t *restrict i2c_dev, const unsigned char data,
        const unsigned char reg);

/**
 * @brief i2c_read_byte 从IIC设备读出一个字节的数据
 * @param i2c_dev IIC设备
 * @param reg IIC设备的寄存器
 * @returns 返回写取的状态, 成功返回数据
 */
extern unsigned char i2c_read_byte(const i2c_dev_t *restrict i2c_dev, const unsigned char reg);

/**
 * @brief i2c_write_bytes 向IIC设备写多个字节的数据
 * @param i2c_dev IIC设备
 * @param buf 需要写入的数据的首地址
 * @param len 写入的字节个数
 * @param reg IIC设备的寄存器
 * @returns 返回写入的状态, 成功返回I2C_STATUS_OK, 失败返回I2C_STATUS_FAILED
 */
extern unsigned char i2c_write_bytes(const i2c_dev_t *restrict i2c_dev, const unsigned char *restrict buf,
    const unsigned char len, const unsigned char reg);

/**
 * @brief i2c_read_bytes 从IIC设备读取多个字节的数据
 * @param i2c_dev IIC设备
 * @param buf 存放被读出的数据的首地址
 * @param len 读出的字节个数
 * @param reg IIC设备的寄存器
 * @returns 返回读取的状态, 成功返回I2C_STATUS_OK, 失败返回I2C_STATUS_FAILED
 */
extern unsigned char i2c_read_bytes(const i2c_dev_t *restrict i2c_dev, unsigned char *restrict buf,
    const unsigned char len, const unsigned char reg);

/**
 * @brief i2c_read_bytes_direct 从IIC设备读取多个字节的数据, 不指定寄存器
 * @param i2c_dev IIC设备
 * @param buf 存放被读出的数据的首地址
 * @param len 读出的字节个数
 * @returns 返回读取的状态, 成功返回I2C_STATUS_OK, 失败返回I2C_STATUS_FAILED
 */
extern unsigned char i2c_read_bytes_direct(const i2c_dev_t *restrict i2c_dev, unsigned char *restrict buf,
    const unsigned char len);

/**
 * @brief i2c_read_byte_direct 从IIC设备读取1个字节的数据, 不指定寄存器
 * @param i2c_dev IIC设备
 * @returns 返回读取的数值
 */
extern unsigned char i2c_read_byte_direct(const i2c_dev_t *restrict i2c_dev);

/**
 * @brief i2c_write_byte_direct 直接向IIC设备发送1个字节的数据, 不指定寄存器
 * @param i2c_dev IIC设备
 * @param data 数据
 * @returns 返回写入的状态, 成功返回I2C_STATUS_OK, 失败返回I2C_STATUS_FAILED
 */
extern unsigned char i2c_write_byte_direct(const i2c_dev_t *restrict i2c_dev, const unsigned char data);

/**
 * @brief i2c_write_bytes_direct 向IIC设备发送多个字节的数据, 不指定寄存器
 * @param i2c_dev IIC设备
 * @param buf 数据的首地址
 * @param len 字节个数
 * @returns 返回写入的状态, 成功返回I2C_STATUS_OK, 失败返回I2C_STATUS_FAILED
 */
extern unsigned char i2c_write_bytes_direct(const i2c_dev_t *restrict i2c_dev, const unsigned char *restrict buf,
    const unsigned char len);

#endif /* __BSP_STM32F767_ATK_APLLOTK_APLLO_IIC_H__ */
