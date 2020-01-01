#include <string.h>
#include <main.h>

void lcd_show(void)
{
    char str[29];
    lcd_char_config_t lcd_str;

    lcd_str.font_config.font = &ascii_fonts[1];
    lcd_str.font_config.mode = FONT_DISPLAY_WITH_BACK_COLOR;
    lcd_str.font_config.backclr = LCD_BG_COLOR;
    lcd_str.point_config.Color = 0x0;
    lcd_str.point_config.Layer = LCD_LAYER_1;
    lcd_str.point_config.pos.x = 0;
    lcd_str.point_config.pos.y = 0;
    lcd_str.point_config.Direction = LCD_DIRECTION_V;

    sprintf(str, "Liu Jiaying");
    stm32f767_atk_apllo_lcd_puts(&lcd_str, str);
}

int main(void)
{
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
        if (rng_ready()) {
            printf("random number = %u\r\n", rng_get_random());
        }
    }

    return 0;
}
