#include <string.h>
#include <main.h>

int main(void)
{
    struct {
        short x, y, z;
    } g, a, m;

    system_init(115200);
    rng_init();
    stm32f767_atk_apllo_sdram_init();
    stm32f767_atk_apllo_lcd_init();
    stm32f767_atk_apllo_lcd_clear(LCD_BG_COLOR);
    stm32f767_atk_apllo_led_init();
    stm32f767_atk_apllo_iic_init();
    while (1) {
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_ON);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, STM32F767_ATK_APLLO_LED_OFF);
        delay_ms(5000);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_OFF);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, STM32F767_ATK_APLLO_LED_ON);
        delay_ms(5000);
        if (rng_ready())
            printf("random number = %u\r\n", rng_get_random());

        printf("temperature: %.2fC\r\n", stm32f767_atk_apllo_mpu9250_temperature());
        if (stm32f767_atk_apllo_mpu9250_gyroscope(&g.x, &g.y, &g.z))
            printf("gx: %hd, gy: %hd, gz: %hd\r\n", g.x, g.y, g.z);

        if (stm32f767_atk_apllo_mpu9250_accelerometer(&a.x, &a.y, &a.z))
            printf("ax: %hd, ay: %hd, az: %hd\r\n", a.x, a.y, a.z);

        if (stm32f767_atk_apllo_mpu9250_magnetometer(&m.x, &m.y, &m.z))
            printf("mx: %hd, my: %hd, mz: %hd\r\n", m.x, m.y, m.z);
    }

    return 0;
}
