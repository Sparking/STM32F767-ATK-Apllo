#include <string.h>
#include <main.h>

void stm32f767_timer_handler(void)
{
    static unsigned char status = STM32F767_ATK_APLLO_LED_OFF;

    stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, status);
    stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, !status);
    status = (status == STM32F767_ATK_APLLO_LED_OFF) ? STM32F767_ATK_APLLO_LED_ON : STM32F767_ATK_APLLO_LED_OFF;
}

extern int _write(int fd, char *ptr, int len);

void ano_dt_send_status(float angle_rol, float angle_pit, float angle_yaw, int alt, unsigned char fly_model, unsigned char armed)
{
    int value;
    unsigned char sum;
    unsigned char i, j;
    char data_to_send[50];

    i = 0;
    data_to_send[i++] = 0xAA;
    data_to_send[i++] = 0xAA;
    data_to_send[i++] = 0x01;
    data_to_send[i++] = 0;
    value = (int)(angle_rol*100);
    data_to_send[i++] = (value >> 8) & 0xFF;
    data_to_send[i++] = value & 0xFF;
    value = (int)(angle_pit*100);
    data_to_send[i++] = (value >> 8) & 0xFF;
    data_to_send[i++] = value & 0xFF;
    value = (int)(angle_yaw*100);
    data_to_send[i++] = (value >> 8) & 0xFF;
    data_to_send[i++] = value & 0xFF;
    data_to_send[i++] = (alt >> 24) & 0xFF;
    data_to_send[i++] = (alt >> 16) & 0xFF;
    data_to_send[i++] = (alt >> 8) & 0xFF;
    data_to_send[i++] = alt & 0xFF;
    data_to_send[i++] = fly_model;
    data_to_send[i++] = armed;
    data_to_send[3] = i-4;
    for(sum = 0, j = 0; j < i; j++) {
        sum += ((unsigned char *)data_to_send)[j];
    }

    data_to_send[i++] = sum;
    _write(1, data_to_send, i);
}

void ano_dt_send_sensor(hvector3d_t *restrict gyro, hvector3d_t *restrict acc,
    hvector3d_t *restrict mag)
{
    unsigned char sum;
    unsigned char i, j;
    char data_to_send[254];

    i = 0;
    /* 帧头 */
    data_to_send[i++] = 0xAA;
    data_to_send[i++] = 0xAA;

    /* 功能字 */
    data_to_send[i++] = 0x02;
    data_to_send[i++] = 18;

    /* */
    data_to_send[i++] = (acc->x >> 8) & 0xFF;
    data_to_send[i++] = (acc->x) & 0xFF;
    data_to_send[i++] = (acc->y >> 8) & 0xFF;
    data_to_send[i++] = (acc->y) & 0xFF;
    data_to_send[i++] = (acc->z >> 8) & 0xFF;
    data_to_send[i++] = (acc->z) & 0xFF;
    data_to_send[i++] = (gyro->x >> 8) & 0xFF;
    data_to_send[i++] = (gyro->x) & 0xFF;
    data_to_send[i++] = (gyro->y >> 8) & 0xFF;
    data_to_send[i++] = (gyro->y) & 0xFF;
    data_to_send[i++] = (gyro->z >> 8) & 0xFF;
    data_to_send[i++] = (gyro->z) & 0xFF;
    data_to_send[i++] = (mag->x >> 8) & 0xFF;
    data_to_send[i++] = (mag->x) & 0xFF;
    data_to_send[i++] = (mag->y >> 8) & 0xFF;
    data_to_send[i++] = (mag->y) & 0xFF;
    data_to_send[i++] = (mag->z >> 8) & 0xFF;
    data_to_send[i++] = (mag->z) & 0xFF;
    for(sum = 0, j = 0; j < i; ++j) {
        sum += ((unsigned char *)data_to_send)[j];
    }
    data_to_send[i++] = sum;

    _write(1, data_to_send, i);
}

void GPIO_EXTI12_Handler(void)
{
    hvector3d_t g_raw;
    hvector3d_t a_raw;
    hvector3d_t m_raw;
    fvector3d_t g;
    fvector3d_t a;
    fvector3d_t m;
    fvector3d_t avg_g;
    euler_angle_t angle;
    unsigned int data;
    static fvector3d_t last_g = {0.0f, 0.0f, 0.0f};
    static quaternion_t q = {1.0f, 0.0f, 0.0f, 0.0f};

    data = stm32f767_atk_apllo_pcf8574_read();
    m_raw.x = m_raw.y = m_raw.z = 0;
    if ((data & (1 << 6))) {
        stm32f767_atk_apllo_mpu9250_gyroscope(&g, &g_raw);
        stm32f767_atk_apllo_mpu9250_accelerometer(&a, &a_raw);
        stm32f767_atk_apllo_mpu9250_magnetometer(&m, &m_raw);
        fvector3d_add(&avg_g, &g, &last_g);
        fvector3d_scale(&avg_g, &avg_g, 0.5f * 0.001 * degrees2radians(1.0f));
        quaternion_update(&q, &avg_g);
        quaternion_to_euler(&angle, &q);
        last_g = g;
#ifndef GYRO_CALIBRATION
#if 1
        ano_dt_send_status(angle.pitch, angle.roll, angle.yaw, 0, 0, 1);
        ano_dt_send_sensor(&g_raw, &a_raw, &m_raw);
#endif
#if 0
        printf("temperature: %.2fC\r\n", stm32f767_atk_apllo_mpu9250valueerature());
        printf("gx: %f, gy: %f, gz: %f\r\n", g.x, g.y, g.z);
        printf("ax: %f, ay: %f, az: %f\r\n", a.x, a.y, a.z);
        printf("mx: %f, my: %f, mz: %f\r\n", m.x, m.y, m.z);
        printf("pitch: %f, roll: %f, yaw: %f\r\n", angle.pitch, angle.roll, angle.yaw);
#endif
#endif
    }
}

int main(void)
{
    system_init(115200);
    stm32f767_atk_apllo_sdram_init();
    stm32f767_atk_apllo_lcd_init();
    stm32f767_atk_apllo_lcd_clear(LCD_BG_COLOR);
    stm32f767_atk_apllo_led_init();
    stm32f767_atk_apllo_iic_init();

    while (1) {
        delay_ms(1000);
    }

    return 0;
}
