#include <string.h>
#include <main.h>

void stm32f767_timer_handler(void)
{
    static unsigned char status = STM32F767_ATK_APLLO_LED_OFF;

    stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, status);
    status = (status == STM32F767_ATK_APLLO_LED_OFF) ? STM32F767_ATK_APLLO_LED_ON : STM32F767_ATK_APLLO_LED_OFF;
}

extern int _write(int fd, char *ptr, int len);
static unsigned char data_to_send[50];
static void ano_dt_send_data(void *ptr, int len)
{
    _write(1, ptr, len);
}

void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, int alt, unsigned char fly_model, unsigned char armed)
{
	unsigned char _cnt = 0;
	int _temp = alt;

	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0;

	_temp = (int)(angle_rol*100);
	data_to_send[_cnt++]=(_temp >> 8) & 0xFF;
	data_to_send[_cnt++]=_temp & 0xFF;
	_temp = (int)(angle_pit*100);
	data_to_send[_cnt++]=(_temp >> 8) & 0xFF;
	data_to_send[_cnt++]=_temp & 0xFF;
	_temp = (int)(angle_yaw*100);
	data_to_send[_cnt++]=(_temp >> 8) & 0xFF;
	data_to_send[_cnt++]=_temp & 0xFF;

	data_to_send[_cnt++]=(_temp >> 24) & 0xFF;
	data_to_send[_cnt++]=(_temp >> 16) & 0xFF;
	data_to_send[_cnt++]=(_temp >> 8) & 0xFF;
	data_to_send[_cnt++]=_temp & 0xFF;

	data_to_send[_cnt++] = fly_model;

	data_to_send[_cnt++] = armed;

	data_to_send[3] = _cnt-4;

	int sum = 0;
	for(int i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++]=sum;

	ano_dt_send_data(data_to_send, _cnt);
}

void GPIO_EXTI12_Handler(void)
{
    fvector3d_t g;
    fvector3d_t a;
    fvector3d_t m;
    euler_angle angle;
    unsigned int data;
    static quaternion_t q = {1.0f, 0.0f, 0.0f, 0.0f};

    data = stm32f767_atk_apllo_pcf8574_read();
    if ((data & (1 << 6))) {
        //printf("temperature: %.2fC\r\n", stm32f767_atk_apllo_mpu9250_temperature());
        stm32f767_atk_apllo_mpu9250_gyroscope(&g);
        //printf("gx: %f, gy: %f, gz: %f\r\n", g.x, g.y, g.z);
        stm32f767_atk_apllo_mpu9250_accelerometer(&a);
        //printf("ax: %f, ay: %f, az: %f\r\n", a.x, a.y, a.z);
        stm32f767_atk_apllo_mpu9250_magnetometer(&m);
        //printf("mx: %f, my: %f, mz: %f\r\n", m.x, m.y, m.z);
        quaternion_update(&q, 0.001, g.x, g.y, g.z);
        quaternion_to_euler(&angle, &q);
        ANO_DT_Send_Status(angle.pitch, angle.roll, angle.yaw, 0, 0, 1);
        //printf("pitch: %f, roll: %f, yaw: %f\r\n", angle.pitch, angle.roll, angle.yaw);
    }
}

int main(void)
{
    int i;

    system_init(115200);
    stm32f767_atk_apllo_sdram_init();
    stm32f767_atk_apllo_lcd_init();
    stm32f767_atk_apllo_lcd_clear(LCD_BG_COLOR);
    stm32f767_atk_apllo_led_init();
    stm32f767_atk_apllo_iic_init();

    for (i = 0; i < 5; ++i) {
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_ON);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, STM32F767_ATK_APLLO_LED_ON);
        delay_ms(1000);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, STM32F767_ATK_APLLO_LED_OFF);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_OFF);
        delay_ms(1000);
    }

    stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_ON);
    while (1) {
        delay_ms(1);
    }

    return 0;
}
