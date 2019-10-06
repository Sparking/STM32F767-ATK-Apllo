#include <main.h>

static i2c_bus_t i2c_bus[1];
static i2c_dev_t i2c_dev[1];

int main(void)
{
    system_init(115200);
    i2c_bus_init(&i2c_bus[0], GPIOA, GPIO_PIN_7, GPIOA, GPIO_PIN_10);
    i2c_init(&i2c_dev[0], &i2c_bus[0], 0x41);

    while (1) {
        printf("Hello World!\r\n");
        delay_ms(1000);
    }

    return 0;
}