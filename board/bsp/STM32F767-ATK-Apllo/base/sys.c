#include <rng.h>
#include <sys.h>
#include <timer.h>

UART_HandleTypeDef huart1 = {0};

void uart_wrch(USART_TypeDef *uart, char ch)
{
    while (!(uart->ISR & 0x40))
        continue;
    uart->TDR = ch;
}

/* 标准输入输出重定向函数 */
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

void Error_Handler(void)
{
    while (1)
        continue;
}

void Cache_Enable(void)
{
    SCB_EnableICache();//使能I-Cache
    SCB_EnableDCache();//使能D-Cache    
	SCB->CACR|=1<<2;   //强制D-Cache透写,如不开启,实际使用中可能遇到各种问题	
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
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
}

void system_init(const unsigned int baudrate)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure LSE Drive Capability 
     */
    HAL_PWR_EnableBkUpAccess();
    /** Configure the main internal regulator output voltage 
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the CPU, AHB and APB busses clocks 
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Activate the Over-Drive mode 
     */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks 
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_TIM;
    PeriphClkInitStruct.TIMPresSelection = RCC_TIMPRES_ACTIVATED;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }

    Cache_Enable();
    rng_init();
    uart1_init(baudrate);
    timer_init(10000 - 1, 10800 - 1);
}
