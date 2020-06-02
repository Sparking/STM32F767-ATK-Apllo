#include "sys.h"
#include "iic.h"

#define I2C_SIGNAL_NOACK    GPIO_PIN_SET
#define I2C_SIGNAL_ACK      GPIO_PIN_RESET

#if 1
#define I2C_MUTEX_LOCK(dev) \
    xSemaphoreTake((dev)->bus->mutex, portMAX_DELAY)
#define I2C_MUTEX_UNLOCK(dev) \
    xSemaphoreGive((dev)->bus->mutex)
#else
#define I2C_MUTEX_LOCK(dev)
#define I2C_MUTEX_UNLOCK(dev)
#endif
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

void i2c_bus_init(i2c_bus_t *restrict bus, GPIO_TypeDef *scl_gpio, const uint16_t scl_pin,
        GPIO_TypeDef *sda_gpio, const uint16_t sda_pin)
{
    GPIO_InitTypeDef GPIO_Initure;

    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_FAST;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Alternate = 0;

    gpio_clk_enable(scl_gpio);
    gpio_clk_enable(sda_gpio);

    GPIO_Initure.Pin  = scl_pin;
    HAL_GPIO_Init(scl_gpio, &GPIO_Initure);

    GPIO_Initure.Pin = sda_pin;
    HAL_GPIO_Init(sda_gpio, &GPIO_Initure);

    gpio_set_pin(scl_gpio, scl_pin);
    gpio_set_pin(sda_gpio, sda_pin);

    bus->scl.gpio = scl_gpio;
    bus->scl.pin = scl_pin;
    bus->sda.gpio = sda_gpio;
    bus->sda.pin = sda_pin;
    bus->mutex = xSemaphoreCreateMutex();
}

void i2c_start(const i2c_dev_t *restrict i2c_dev)
{
    I2C_SDA_MODE_OUT(i2c_dev);
    I2C_SET_SDA(i2c_dev);
    I2C_SET_SCL(i2c_dev);
    delay_us(4);
    I2C_RESET_SDA(i2c_dev);
	delay_us(4);
    I2C_RESET_SCL(i2c_dev);
}

void i2c_stop(const i2c_dev_t *restrict i2c_dev)
{
    I2C_SDA_MODE_OUT(i2c_dev);
	I2C_RESET_SDA(i2c_dev);
    I2C_SET_SCL(i2c_dev);
    delay_us(4);
	I2C_SET_SDA(i2c_dev);
	delay_us(4);
	I2C_RESET_SCL(i2c_dev);
}

unsigned char i2c_wait_ack(const i2c_dev_t *restrict i2c_dev)
{
    unsigned char ucErrTime;

    ucErrTime = 0;
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

void i2c_send_ack(const i2c_dev_t *restrict i2c_dev, const GPIO_PinState ack)
{
    I2C_RESET_SCL(i2c_dev);
    I2C_SDA_MODE_OUT(i2c_dev);
    I2C_WRITE_SDA(i2c_dev, ack);
    delay_us(2);
    I2C_SET_SCL(i2c_dev);
    delay_us(2);
    I2C_RESET_SCL(i2c_dev);
}

void i2c_base_send_byte(const i2c_dev_t *restrict i2c_dev, unsigned char data)
{
    unsigned char t;

    I2C_SDA_MODE_OUT(i2c_dev);
    I2C_RESET_SCL(i2c_dev);
    for(t = 0; t < 8; t++) {
        I2C_WRITE_SDA(i2c_dev, data >> 7 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;
        I2C_SET_SCL(i2c_dev);
        delay_us(2);
        I2C_RESET_SCL(i2c_dev);
        delay_us(2);
    }
}

unsigned char i2c_base_read_byte(const i2c_dev_t *restrict i2c_dev, const GPIO_PinState ack)
{
    unsigned char i, receive;

    I2C_SDA_MODE_IN(i2c_dev);
    for(receive = 0, i = 0; i < 8; i++) {
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

unsigned char i2c_write_byte(const i2c_dev_t *restrict i2c_dev, const unsigned char data, const unsigned char reg)
{
    unsigned char ret;

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
    if (i2c_wait_ack(i2c_dev)) {
        ret = I2C_STATUS_FAILED;
    }

stop_i2c_write_byte:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);

    return ret;
}

unsigned char i2c_read_byte(const i2c_dev_t *restrict i2c_dev, const unsigned char reg)
{
    unsigned char data;

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

unsigned char i2c_read_bytes(const i2c_dev_t *restrict i2c_dev, unsigned char *restrict buf,
    const unsigned char len, const unsigned char reg)
{
    unsigned char i, ret;
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
        if (i == len - 1) {
            ack_signal = I2C_SIGNAL_NOACK;
        }

        buf[i] = i2c_base_read_byte(i2c_dev, ack_signal);
    }

stop_i2c_read_bytes:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);

    return ret;
}

unsigned char i2c_write_bytes(const i2c_dev_t *restrict i2c_dev, const unsigned char *restrict buf,
    const unsigned char len, const unsigned char reg)
{
    unsigned char i, ret;

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

unsigned char i2c_write_bytes_direct(const i2c_dev_t *restrict i2c_dev, const unsigned char *restrict buf,
    const unsigned char len)
{
    unsigned char ret, i;

    I2C_MUTEX_LOCK(i2c_dev);
    ret = I2C_STATUS_OK;
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr);
    if (i2c_wait_ack(i2c_dev)) {
        ret = I2C_STATUS_FAILED;
        goto i2c_read_bytes_direct_exit;
    }

    for (i = 0; i < len; i++) {
        i2c_base_send_byte(i2c_dev, buf[i]);
    }

i2c_read_bytes_direct_exit:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);

    return ret;
}

unsigned char i2c_write_byte_direct(const i2c_dev_t *restrict i2c_dev, const unsigned char data)
{
    return i2c_write_bytes_direct(i2c_dev, &data, 1);
}

unsigned char i2c_read_bytes_direct(const i2c_dev_t *restrict i2c_dev, unsigned char *restrict buf,
    const unsigned char len)
{
    unsigned char ret, i;
    GPIO_PinState ack_signal;

    I2C_MUTEX_LOCK(i2c_dev);
    ret = I2C_STATUS_OK;
    i2c_start(i2c_dev);
    i2c_base_send_byte(i2c_dev, i2c_dev->addr | 1);
    if (i2c_wait_ack(i2c_dev)) {
        ret = I2C_STATUS_FAILED;
        goto i2c_read_bytes_direct_exit;
    }

    ack_signal = I2C_SIGNAL_ACK;
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            ack_signal = I2C_SIGNAL_NOACK;
        }

        buf[i] = i2c_base_read_byte(i2c_dev, ack_signal);
    }

i2c_read_bytes_direct_exit:
    i2c_stop(i2c_dev);
    I2C_MUTEX_UNLOCK(i2c_dev);

    return ret;
}

unsigned char i2c_read_byte_direct(const i2c_dev_t *restrict i2c_dev)
{
    unsigned char data;

    data = 0xFF;
    i2c_read_bytes_direct(i2c_dev, &data, 1);

    return data;
}

