#include "sys.h"
#include "stm32f767-atk-apllo-mpu9250.h"

/* Registers of MPU9250 */
#define MPU_SELF_TEST_X_GYRO    0x00
#define MPU_SELF_TEST_Y_GYRO    0x01
#define MPU_SELF_TEST_Z_GYRO    0x02
#define MPU_SELF_TEST_X_ACCEL   0x0D
#define MPU_SELF_TEST_Y_ACCEL   0x0E
#define MPU_SELF_TEST_Z_ACCEL   0x0F
#define MPU_XG_OFFSET_H         0x13
#define MPU_XG_OFFSET_L         0x14
#define MPU_YG_OFFSET_H         0x15
#define MPU_YG_OFFSET_L         0x16
#define MPU_ZG_OFFSET_H         0x17
#define MPU_ZG_OFFSET_L         0x18
#define MPU_SMPLRT_DIV          0x19
#define MPU_CONFIG              0x1A
#define MPU_GYRO_CONFIG         0x1B
#define MPU_ACCEL_CONFIG        0x1C
#define MPU_ACCEL_CONFIG2       0x1D
#define MPU_LP_ACCEL_ODR        0x1E
#define MPU_WOM_THR             0x1F
#define MPU_FIFO_EN             0x23
#define MPU_I2C_MST_CTRL        0x24
#define MPU_I2C_SLV0_ADDR       0x25
#define MPU_I2C_SLV0_REG        0x26
#define MPU_I2C_SLV0_CTRL       0x27
#define MPU_I2C_SLV1_ADDR       0x28
#define MPU_I2C_SLV1_REG        0x29
#define MPU_I2C_SLV1_CTRL       0x2A
#define MPU_I2C_SLV2_ADDR       0x2B
#define MPU_I2C_SLV2_REG        0x2C
#define MPU_I2C_SLV2_CTRL       0x2D
#define MPU_I2C_SLV3_ADDR       0x2E
#define MPU_I2C_SLV3_REG        0x2F
#define MPU_I2C_SLV3_CTRL       0x30
#define MPU_I2C_SLV4_ADDR       0x31
#define MPU_I2C_SLV4_REG        0x32
#define MPU_I2C_SLV4_DO         0x33
#define MPU_I2C_SLV4_CTRL       0x34
#define MPU_I2C_SLV4_DI         0x35
#define MPU_I2C_MST_STATUS      0x36
#define MPU_INT_PIN_CFG         0x37
#define MPU_INT_ENABLE          0x38
#define MPU_INT_STATUS          0x3A
#define MPU_ACCEL_XOUT_H        0x3B
#define MPU_ACCEL_XOUT_L        0x3C
#define MPU_ACCEL_YOUT_H        0x3D
#define MPU_ACCEL_YOUT_L        0x3E
#define MPU_ACCEL_ZOUT_H        0x3F
#define MPU_ACCEL_ZOUT_L        0x40
#define MPU_TEMP_OUT_H          0x41
#define MPU_TEMP_OUT_L          0x42
#define MPU_GYRO_XOUT_H         0x43
#define MPU_GYRO_XOUT_L         0x44
#define MPU_GYRO_YOUT_H         0x45
#define MPU_GYRO_YOUT_L         0x46
#define MPU_GYRO_ZOUT_H         0x47
#define MPU_GYRO_ZOUT_L         0x48
#define MPU_EXT_SENS_DATA_00    0x49
#define MPU_EXT_SENS_DATA_01    0x4A
#define MPU_EXT_SENS_DATA_02    0x4B
#define MPU_EXT_SENS_DATA_03    0x4C
#define MPU_EXT_SENS_DATA_04    0x4D
#define MPU_EXT_SENS_DATA_05    0x4E
#define MPU_EXT_SENS_DATA_06    0x4F
#define MPU_EXT_SENS_DATA_07    0x50
#define MPU_EXT_SENS_DATA_08    0x51
#define MPU_EXT_SENS_DATA_09    0x52
#define MPU_EXT_SENS_DATA_10    0x53
#define MPU_EXT_SENS_DATA_11    0x54
#define MPU_EXT_SENS_DATA_12    0x55
#define MPU_EXT_SENS_DATA_13    0x56
#define MPU_EXT_SENS_DATA_14    0x57
#define MPU_EXT_SENS_DATA_15    0x58
#define MPU_EXT_SENS_DATA_16    0x59
#define MPU_EXT_SENS_DATA_17    0x5A
#define MPU_EXT_SENS_DATA_18    0x5B
#define MPU_EXT_SENS_DATA_19    0x5C
#define MPU_EXT_SENS_DATA_20    0x5D
#define MPU_EXT_SENS_DATA_21    0x5E
#define MPU_EXT_SENS_DATA_22    0x5F
#define MPU_EXT_SENS_DATA_23    0x60
#define MPU_I2C_SLV0_DO         0x63
#define MPU_I2C_SLV1_DO         0x64
#define MPU_I2C_SLV2_DO         0x65
#define MPU_I2C_SLV3_DO         0x66
#define MPU_I2C_MST_DELAY_CTRL  0x67
#define MPU_SIGNAL_PATH_RESET   0x68
#define MPU_MOT_DETECT_CTRL     0x69
#define MPU_USER_CTRL           0x6A
#define MPU_PWR_MGMT1           0x6B
#define MPU_PWR_MGMT2           0x6C
#define MPU_FIFO_COUNTH         0x72
#define MPU_FIFO_COUNTL         0x73
#define MPU_FIFO_R_W            0x74
#define MPU_WHO_AM_I            0x75
#define MPU_XA_OFFSET_H         0x77
#define MPU_XA_OFFSET_L         0x78
#define MPU_YA_OFFSET_H         0x7A
#define MPU_YA_OFFSET_L         0x7B
#define MPU_ZA_OFFSET_H         0x7D
#define MPU_ZA_OFFSET_L         0x7E

#define AK8963_REG_WIA          0x00    /* Device ID */
#define AK8963_REG_INFO         0x01    /* Information */
#define AK8963_REG_ST1          0x02    /* Status 1 */
#define AK8963_REG_HXL          0x03
#define AK8963_REG_HXH          0x04
#define AK8963_REG_HYL          0x05
#define AK8963_REG_HYH          0x06
#define AK8963_REG_HZL          0x07
#define AK8963_REG_HZH          0x08
#define AK8963_REG_ST2          0x09    /* Status 2 */
#define AK8963_REG_CNTL1        0x0A    /* Control 1 */
#define AK8963_REG_CNTL2        0x0B    /* Control 2 */
#define AK8963_REG_ASTC         0x0C    /* Self-test */
#define AK8963_REG_TS1          0x0D    /* Test 1 */
#define AK8963_REG_TS2          0x0E    /* Test 2 */
#define AK8963_REG_I2CDIS       0x0F    /* I2C Disable */
#define AK8963_REG_ASAX         0x10    /* X-axis sensitivity adjustment value */
#define AK8963_REG_ASAY         0x11    /* Y-axis sensitivity adjustment value */
#define AK8963_REG_ASAZ         0x12    /* Z-axis sensitivity adjustment value */
#define AK8963_REG_RSV          0x13    /* Reserved */

#define MPU920_DEVICE_ID        0x71
#define AK8963_DEVICE_ID        0x48

static i2c_dev_t i2c_mpu9250;
static i2c_dev_t i2c_ak8963;
const static fvector3d_t mpu9250_gyro_bias = {
    .x = 8.44f,
    .y = 1.3f,
    .z = -6.78f
};
static hvector3d_t mag_gain;

bool stm32f767_atk_apllo_mpu9250_init(const i2c_bus_t *restrict bus)
{
    unsigned char value;
    unsigned char calibration[3];

    /* 初始化6轴传感器I2C总线 */
    i2c_init(&i2c_mpu9250, bus, 0x68);
    /* 初始化3轴磁强计I2C总线 */
    i2c_init(&i2c_ak8963, bus, 0x0C);
    i2c_write_byte(&i2c_mpu9250, 0x80, MPU_PWR_MGMT1);      /* 解除休眠 */
    delay_ms(100);                                          /* 延时保证复位 */
    value = i2c_read_byte(&i2c_mpu9250, MPU_WHO_AM_I);      /* 读取MPU9250的设备ID */
    if (value != MPU920_DEVICE_ID) {
        return false;
    }

    /* 复位陀螺仪、加速计和温度计 */
    i2c_write_byte(&i2c_mpu9250, 0x07, MPU_SIGNAL_PATH_RESET);
    delay_ms(100);
    i2c_write_byte(&i2c_mpu9250, 0x00, MPU_PWR_MGMT1);
    delay_ms(100);
    i2c_write_byte(&i2c_mpu9250, 0x01, MPU_PWR_MGMT1);      /* 选择PLL时钟源 */
    delay_ms(15);
    i2c_write_byte(&i2c_mpu9250, 0x18, MPU_GYRO_CONFIG);    /* 陀螺仪量程+-2000dps */
    delay_ms(15);
    i2c_write_byte(&i2c_mpu9250, 0x18, MPU_ACCEL_CONFIG);   /* 加速计量程+-16g */
    delay_ms(15);
    i2c_write_byte(&i2c_mpu9250, 0x00, MPU_CONFIG);         /* 采样频率8kHz,带宽250Hz */
    delay_ms(15);
    i2c_write_byte(&i2c_mpu9250, 0x00, MPU_SMPLRT_DIV);     /* 不起作用 */
    delay_ms(100);
    i2c_write_byte(&i2c_mpu9250, 0x12, MPU_INT_PIN_CFG);    /* BYPASS, INT_ANYRD_2CLEAR */
    delay_ms(15);

#if 1
    i2c_write_byte(&i2c_mpu9250, 0x01, MPU_INT_ENABLE);
    delay_ms(15);
#endif

    value = i2c_read_byte(&i2c_ak8963, AK8963_REG_WIA);
    if (value != AK8963_DEVICE_ID) {
        return false;
    }

    i2c_write_byte(&i2c_ak8963, 0x00, AK8963_REG_CNTL1);
    delay_ms(20);
    i2c_write_byte(&i2c_ak8963, 0x0F, AK8963_REG_CNTL1);
    delay_ms(10);
    i2c_read_bytes(&i2c_ak8963, calibration, 3, AK8963_REG_ASAX);
    delay_ms(10);
    mag_gain.x = (char)(calibration[0] + 128);
    mag_gain.y = (char)(calibration[1] + 128);
    mag_gain.z = (char)(calibration[2] + 128);
    i2c_write_byte(&i2c_ak8963, 0x00, AK8963_REG_CNTL1);
    delay_ms(10);
    (void)i2c_read_byte(&i2c_ak8963, AK8963_REG_ST1);
    (void)i2c_read_byte(&i2c_ak8963, AK8963_REG_ST2);
    i2c_write_byte(&i2c_ak8963, 0x11, AK8963_REG_CNTL1);

    return true;
}

float stm32f767_atk_apllo_mpu9250_temperature(void)
{
    float temp;
    short raw;
    unsigned char buf[2];

    temp = 0.0f;
    if (i2c_read_bytes(&i2c_mpu9250, buf, 2, MPU_TEMP_OUT_H) == I2C_STATUS_OK) {
        raw = ((unsigned short)buf[0] << 8) | buf[1];
        temp = 21.0f + ((double)raw) / 333.87f;
    }

    return temp;
}

bool stm32f767_atk_apllo_mpu9250_gyroscope(fvector3d_t *restrict gyro, hvector3d_t *restrict raw)
{
    unsigned char buf[6], res;

    res = false;
    if (i2c_read_bytes(&i2c_mpu9250, buf, 6, MPU_GYRO_XOUT_H) == I2C_STATUS_OK) {
        raw->x = ((unsigned short)buf[0] << 8) | buf[1];
        raw->y = ((unsigned short)buf[2] << 8) | buf[3];
        raw->z = ((unsigned short)buf[4] << 8) | buf[5];
        gyro->x = (float)raw->x;
        gyro->y = (float)raw->y;
        gyro->z = (float)raw->z;
        fvector3d_add(gyro, gyro, &mpu9250_gyro_bias);
        fvector3d_scale(gyro, gyro, 1.0f / 16.4f);
        res = true;
    }

    return res;
}

bool stm32f767_atk_apllo_mpu9250_accelerometer(fvector3d_t *restrict acc, hvector3d_t *restrict raw)
{
    unsigned char buf[6], res;

    res = false;
    if (i2c_read_bytes(&i2c_mpu9250, buf, 6, MPU_ACCEL_XOUT_H) == I2C_STATUS_OK) {
        raw->x = ((unsigned short)buf[0] << 8) | buf[1];
        raw->y = ((unsigned short)buf[2] << 8) | buf[3];
        raw->z = ((unsigned short)buf[4] << 8) | buf[5];
        acc->x = (float)raw->x;
        acc->y = (float)raw->y;
        acc->z = (float)raw->z;
        fvector3d_scale(acc, acc, 1.0f / 2048.0f);
        res = true;
    }

    return res;
}

bool stm32f767_atk_apllo_mpu9250_magnetometer(fvector3d_t *restrict mag, hvector3d_t *restrict raw)
{
    unsigned char buf[6], res;

    res = false;
    if (i2c_read_bytes(&i2c_ak8963, buf, 6, AK8963_REG_HXL) == I2C_STATUS_OK) {
        raw->x = ((unsigned short)buf[0] << 8) | buf[1];
        raw->y = ((unsigned short)buf[2] << 8) | buf[3];
        raw->z = ((unsigned short)buf[4] << 8) | buf[5];
        mag->x = (float)(raw->x * mag_gain.x);
        mag->y = (float)(raw->y * mag_gain.y);
        mag->z = (float)(raw->z * mag_gain.z);
        fvector3d_scale(mag, mag, 1.0f / (600000.0f * 256.0f));
        i2c_write_byte(&i2c_ak8963, 0x11, AK8963_REG_CNTL1);
        res = true;
    }

    return res;
}
