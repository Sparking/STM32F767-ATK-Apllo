#include <main.h>

int main(void)
{
    system_init(115200);
    stm32f767_atk_apllo_iic_init();
    stm32f767_atk_apllo_led_init();

    printf("Hello World!\r\n");
    printf("at24c02[0] = %d\r\n", (int)at24c02_read_byte(0));
    while (1) {
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_ON);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, STM32F767_ATK_APLLO_LED_OFF);
        delay_ms(5000);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_GREEN, STM32F767_ATK_APLLO_LED_OFF);
        stm32f767_atk_apllo_led_set(STM32F767_ATK_APLLO_LED_RED, STM32F767_ATK_APLLO_LED_ON);
        delay_ms(5000);
    }

    return 0;
}
