#include "sys.h"
#include "iic.h"

#define I2C_MUTEX_LOCK(dev) \
    xSemaphoreTake((dev)->bus->mutex, portMAX_DELAY)
#define I2C_MUTEX_UNLOCK(dev) \
    xSemaphoreGive((dev)->bus->mutex)
#define I2C_READ_SDA(dev) \
    HAL_GPIO_ReadPin((dev)->bus->sda.gpio, (dev)->bus->sda.pin)
#define I2C_WRITE_SDA(dev, bit) \
    HAL_GPIO_WritePin((dev)->bus->sda.gpio, (dev)->bus->sda.pin, (bit))
#define I2C_SET_SDA(dev) \
    gpio_set_pin((dev)->bus->sda.gpio, (dev)->bus->sda.pin)
#define I2C_RESET_SDA(dev) \
    gpio_reset_pin((dev)->bus->sda.gpio, (dev)->bus->sda.pin)
#define I2C_SET_SCL(dev) \
    gpio_set_pin((dev)->bus->scl.gpio, (dev)->bus->scl.pin)
#define I2C_RESET_SCL(dev) \
    gpio_reset_pin((dev)->bus->scl.gpio, (dev)->bus->scl.pin)
#define I2C_SDA_MODE_OUT(dev) \
    gpio_switch_io_mode((dev)->bus->sda.gpio, (dev)->bus->sda.pin, GPIO_MODE_OUTPUT_PP)
#define I2C_SDA_MODE_IN(dev) \
    gpio_switch_io_mode((dev)->bus->sda.gpio, (dev)->bus->sda.pin, GPIO_MODE_INPUT)

#define I2C_SIGNAL_NOACK    GPIO_PIN_SET
#define I2C_SIGNAL_ACK      GPIO_PIN_RESET

void i2c_bus_init(i2c_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_Initure;

    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_FAST;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Alternate = 0;
    gpio_clk_enable(bus->scl.gpio);
    gpio_clk_enable(bus->sda.gpio);
    GPIO_Initure.Pin  = bus->scl.pin;
    HAL_GPIO_Init(bus->scl.gpio, &GPIO_Initure);
    GPIO_Initure.Pin = bus->sda.pin;
    HAL_GPIO_Init(bus->sda.gpio, &GPIO_Initure);
    gpio_set_pin(bus->scl.gpio, bus->scl.pin);
    gpio_set_pin(bus->sda.gpio, bus->sda.pin);

    bus->mutex = xSemaphoreCreateMutex();
}

void i2c_start(const i2c_device_t *__RESTRICT i2c_dev)
{
    I2C_SDA_MODE_OUT(i2c_dev);
    I2C_SET_SDA(i2c_dev);
    I2C_SET_SCL(i2c_dev);
    delay_us(4);
    I2C_RESET_SDA(i2c_dev);
	delay_us(4);
    I2C_RESET_SCL(i2c_dev);
}

void i2c_stop(const i2c_device_t *__RESTRICT i2c_dev)
{
    I2C_SDA_MODE_OUT(i2c_dev);
	I2C_RESET_SDA(i2c_dev);
    I2C_SET_SCL(i2c_dev);
    delay_us(4);
	I2C_SET_SDA(i2c_dev);
	delay_us(4);
	I2C_RESET_SCL(i2c_dev);
}

uint8_t i2c_wait_ack(const i2c_device_t *__RESTRICT i2c_dev)
{
    uint8_t ucErrTime = 0;

    I2C_SET_SDA(i2c_dev);
    I2C_SDA_MODE_IN(i2c_dev);
    delay_us(2);
    I2C_SET_SCL(i2c_dev);
    delay_us(2);
    while (I2C_READ_SDA(i2c_dev)) {
        ucErrTime++;
        if (ucErrTime > 250) {
            i2c_stop(i2c_dev);
            return I2C_STATUS_FAILED;
        }
    }
    I2C_RESET_SCL(i2c_dev);

    return I2C_STATUS_OK;
}

void i2c_send_ack(const i2c_device_t *__RESTRICT i2c_dev, const GPIO_PinState ack)
{
    I2C_RESET_SCL(i2c_dev);
    I2C_SDA_MODE_OUT(i2c_dev);
    I2C_WRITE_SDA(i2c_dev, ack);
    delay_us(2);
    I2C_SET_SCL(i2c_dev);
    delay_us(2);
    I2C_RESET_SCL(i2c_dev);
}

void i2c_base_send_byte(const i2c_device_t *__RESTRICT i2c_dev, uint8_t data)
{
    I2C_SDA_MODE_OUT(i2c_dev);
    I2C_RESET_SCL(i2c_dev);
    for(uint8_t t = 0; t < 8; t++) {
        I2C_WRITE_SDA(i2c_dev, data >> 7 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;
        I2C_SET_SCL(i2c_dev);
        delay_us(2);
        I2C_RESET_SCL(i2c_dev);
        delay_us(2);
    }
}

uint8_t i2c_base_read_byte(const i2c_device_t *__RESTRICT i2c_dev, const GPIO_PinState ack)
{
    uint8_t i, receive = 0;

    I2C_SDA_MODE_IN(i2c_dev);
    for(i = 0; i < 8; i++) {
        I2C_RESET_SCL(i2c_dev);
        delay_us(2);
        I2C_SET_SCL(i2c_dev);
        receive <<= 1;
        if(I2C_READ_SDA(i2c_dev))
            receive++;
        delay_us(2);
    }
    i2c_send_ack(i2c_dev, ack);

    return receive;
}

uint8_t i2c_write_byte(const i2c_device_t *__RESTRICT i2c_dev, const uint8_t reg, const uint8_t data)
{
    uint8_t ret;

    I2C_MUTEX_LOCK(i2c_dev);
    ret = I2C_STATUS_OK;
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr);
    if (i2c_wait_ack(i2c_dev)) {
        ret = I2C_STATUS_FAILED;
        goto stop_i2c_write_byte;
    }
    i2c_base_send_byte(i2c_dev, reg);
    i2c_wait_ack(i2c_dev);
    i2c_base_send_byte(i2c_dev, data);
    if (i2c_wait_ack(i2c_dev))
        ret = I2C_STATUS_FAILED;

stop_i2c_write_byte:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);
    return ret;
}

uint8_t i2c_read_byte(const i2c_device_t *__RESTRICT i2c_dev, const uint8_t reg)
{
    uint8_t data;

    I2C_MUTEX_LOCK(i2c_dev);
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr);
    i2c_wait_ack(i2c_dev);
    i2c_base_send_byte(i2c_dev, reg);
    i2c_wait_ack(i2c_dev);
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr | 0x01);
    i2c_wait_ack(i2c_dev);
    data = i2c_base_read_byte(i2c_dev, I2C_SIGNAL_NOACK);
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);

    return data;
}

uint8_t i2c_read_bytes(const i2c_device_t *__RESTRICT i2c_dev, uint8_t *__RESTRICT buf,
    const uint8_t len, const uint8_t reg)
{
    uint8_t i, ret;
    GPIO_PinState ack_signal;

    I2C_MUTEX_LOCK(i2c_dev);
    ret = I2C_STATUS_OK;
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr);
    if (i2c_wait_ack(i2c_dev)) {
       ret = I2C_STATUS_FAILED;
       goto stop_i2c_read_bytes;
    }
    i2c_base_send_byte(i2c_dev, reg);
    i2c_wait_ack(i2c_dev);
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr | 1);
    i2c_wait_ack(i2c_dev);
    ack_signal = I2C_SIGNAL_ACK;
    for (i = 0; i < len; ++i) {
        if (i == len - 1)
            ack_signal = I2C_SIGNAL_NOACK;
        buf[i] = i2c_base_read_byte(i2c_dev, ack_signal);
    }

stop_i2c_read_bytes:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);
    return ret;
}

uint8_t i2c_write_bytes(const i2c_device_t *__RESTRICT i2c_dev, const uint8_t *__RESTRICT buf,
    const uint8_t len, const uint8_t reg)
{
    uint8_t i, ret;

    I2C_MUTEX_LOCK(i2c_dev);
    ret = I2C_STATUS_OK;
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr);
    if (i2c_wait_ack(i2c_dev)) {
        ret = I2C_STATUS_FAILED;
        goto stop_i2c_write_bytes;
    }
    i2c_base_send_byte(i2c_dev, reg);
    i2c_wait_ack(i2c_dev);
    for (i = 0; i < len; ++i) {
        i2c_base_send_byte(i2c_dev, buf[i]);
        if (i2c_wait_ack(i2c_dev)) {
            ret = I2C_STATUS_FAILED;
            goto stop_i2c_write_bytes;
        }
    }

stop_i2c_write_bytes:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);
    return ret;
}

uint8_t i2c_only_read_bytes(const i2c_device_t *__RESTRICT i2c_dev, uint8_t *__RESTRICT buf,
    const uint8_t len)
{
    uint8_t ret, i;
    GPIO_PinState ack_signal;

    I2C_MUTEX_LOCK(i2c_dev);
    ret = I2C_STATUS_OK;
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr | 1);
    if (i2c_wait_ack(i2c_dev)) {
        ret = I2C_STATUS_FAILED;
        goto stop_i2c_only_read_bytes;
    }
    ack_signal = I2C_SIGNAL_ACK;
    for (i = 0; i < len; i++) {
        if (i == len - 1)
            ack_signal = I2C_SIGNAL_NOACK;
        buf[i] = i2c_base_read_byte(i2c_dev, ack_signal);
    }

stop_i2c_only_read_bytes:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);
    return ret;
}
