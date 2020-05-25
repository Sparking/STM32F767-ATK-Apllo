#include <stm32f7xx_hal.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
 * @{
 */

/**
 * @brief  Initializes the Global MSP.
 * @param  None
 * @retval None
 */
void HAL_MspInit(void)
{
}

/**
 * @brief  DeInitializes the Global MSP.
 * @param  None  
 * @retval None
 */
void HAL_MspDeInit(void)
{
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_Initure;

    GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_FAST;
    if (huart->Instance == USART1) {
        GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_Initure.Alternate = GPIO_AF7_USART1;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);
        __HAL_RCC_USART1_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);
    } else if (huart->Instance == USART2) {
        GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_Initure.Alternate = GPIO_AF7_USART2;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);
        __HAL_RCC_USART2_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);
    }else if (huart->Instance == USART3) {
        GPIO_Initure.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_Initure.Alternate = GPIO_AF7_USART3;
        __HAL_RCC_GPIOB_CLK_ENABLE();
        HAL_GPIO_Init(GPIOB, &GPIO_Initure);
        __HAL_RCC_USART3_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        HAL_NVIC_SetPriority(USART3_IRQn, 3, 3);
    }
}

void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    __HAL_RCC_RNG_CLK_ENABLE();
    __HAL_RNG_ENABLE(hrng);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
