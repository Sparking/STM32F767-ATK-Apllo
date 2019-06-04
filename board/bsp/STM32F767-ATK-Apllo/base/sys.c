#include <sys.h>

UART_HandleTypeDef huart1 = {0};

void uart_wrch(USART_TypeDef *uart, char ch)
{
    while (!(uart->ISR & 0x40))
        continue;
    uart->TDR = ch;
}

#if defined ( __CC_ARM )
#pragma import(__use_no_semihosting)
struct __FILE {
        int handler;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    return ch;
}

#elif defined ( __GNUC__ )
void _ttywrch(int ch) {
    uart_wrch(USART2, ch);
}

int _read (int fd, char *ptr, int len)
{
    return len;
}

int _write(int fd, char *ptr, int len)
{
    for (int i = 0; i < len; ++i)
        _ttywrch(ptr[i]);

    return len;
}
#else
#error "Unknown Complier"
#endif

void delay_us(unsigned int nus)
{
    unsigned int t[2], all_counts, current_counts;

    t[0] = SysTick->VAL;
    for (all_counts = nus * 80, current_counts = 0; current_counts < all_counts;) {
        t[1] = SysTick->VAL;
        if (t[1] > t[0])
            current_counts = current_counts + t[0] + SysTick->LOAD - t[1];
        else
            current_counts = current_counts + t[0] - t[1];
        t[0] = t[1];
    }
}

void delay_ms(unsigned int nms)
{
    while (nms-- > 0)
        delay_us(1000);
}

#define FAC_MS (1000 / configTICK_RATE_HZ)
void delay_xms(unsigned int ms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        if (ms >= FAC_MS)
            vTaskDelay(ms / FAC_MS);
        ms %= FAC_MS;
    }
    delay_us(ms * 1000);
}

unsigned int get_systick_count(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        return xTaskGetTickCount();
    return HAL_GetTick();
}

static void uart1_init(const unsigned int baudrate)
{
    huart1.Instance = USART2;
    huart1.Init.BaudRate = baudrate;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.gState = HAL_UART_STATE_RESET;
    HAL_UART_Init(&huart1);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void system_init(const unsigned int baudrate)
{
    uart1_init(baudrate);
}
