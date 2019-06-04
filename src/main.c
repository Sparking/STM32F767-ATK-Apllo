#include <main.h>

int main(void)
{
    system_init(115200);

    while (1) {
        printf("Hello World!\r\n");
        delay_ms(1000);
    }

	return 0;
}