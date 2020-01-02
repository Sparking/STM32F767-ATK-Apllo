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

static i2c_dev_t i2c_mpu9250_acc;
static i2c_dev_t i2c_mpu9250_mag;

void stm32f767_atk_apllo_mpu9250_init(i2c_bus_t *bus)
{
	unsigned char value;

	i2c_init(&i2c_mpu9250_acc, bus, 0x68);
	i2c_init(&i2c_mpu9250_mag, bus, 0x0C);
	i2c_write_byte(&i2c_mpu9250_acc, 0x80, MPU_PWR_MGMT1);
	delay_ms(100);
	i2c_write_byte(&i2c_mpu9250_acc, 0x00, MPU_PWR_MGMT1);
	// TODO:
	i2c_write_byte(&i2c_mpu9250_acc, 0x00, MPU_INT_ENABLE);
	i2c_write_byte(&i2c_mpu9250_acc, 0x00, MPU_USER_CTRL);
	i2c_write_byte(&i2c_mpu9250_acc, 0x00, MPU_FIFO_EN);
	i2c_write_byte(&i2c_mpu9250_acc, 0x82, MPU_INT_PIN_CFG);
	value = i2c_read_byte(&i2c_mpu9250_acc, MPU_WHO_AM_I);
	if (value == MPU920_DEVICE_ID) {
        printf("Init MPU9250 Success\r\n");
    } else {
        printf("Init MPU9250 Failed\r\n");
    }
	value = i2c_read_byte(&i2c_mpu9250_mag, AK8963_REG_WIA);
    if (value == AK8963_DEVICE_ID) {
        printf("Init AK8963 Success\r\n");
    } else {
        printf("Init AK8963 Failed\r\n");
    }
}

