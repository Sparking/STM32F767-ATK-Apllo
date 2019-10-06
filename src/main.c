#include <main.h>

int main(void)
{
    unsigned char t = 0;
    system_init(115200);
    stm32f767_atk_apllo_iic_init();

    while (1) {
        printf("Hello World!\r\n");
        at24c02_write_byte(t++, 0);
        delay_ms(5); // wait at24c02 writing to be stable
        printf("at24c02_read_byte = %d\r\n", (int)at24c02_read_byte(0));
        delay_ms(5000);
    }

    return 0;
}
