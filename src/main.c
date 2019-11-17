#include <string.h>
#include <main.h>

static char buff[sizeof(struct image) + 640 * 480];

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
    struct image *img = (struct image *)buff;
    img->height = 480;
    img->width = 640;

    system_init(115200);
    rng_init();
    stm32f767_atk_apllo_sdram_init();

    unsigned char *ptr = (unsigned char *)Bank5_SDRAM_ADDR;
    for (int i = 0; i < 480 * 272 * 2; ++i)
        ptr[i] = i % 255;
    for (int i = 0; i < 480 * 272 * 2; ++i)
        if (ptr[i] != i % 255) {
            while (1);
        }

    stm32f767_atk_apllo_lcd_init();
    stm32f767_atk_apllo_lcd_clear(LCD_BG_COLOR);
    stm32f767_atk_apllo_led_init();
    stm32f767_atk_apllo_iic_init();

    lcd_show();
    memset(img->data, 0, 480 * 640);
    printf("Hello World!\r\n");
    printf("at24c02[0] = %d\r\n", (int)at24c02_read_byte(0));
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
