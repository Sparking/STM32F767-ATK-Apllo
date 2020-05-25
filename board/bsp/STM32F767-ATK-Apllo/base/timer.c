#include <stm32f767xx.h>
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_rcc.h>
#include <stm32f7xx_hal_tim.h>
#include "timer.h"

static TIM_HandleTypeDef tim3_handler;

__attribute__((weak)) void stm32f767_timer_handler(void) {}

void timer_init(unsigned short arr, unsigned short psc)
{
    tim3_handler.Instance = TIM3;
    tim3_handler.Init.Prescaler = psc;
    tim3_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim3_handler.Init.Period = arr;
    tim3_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&tim3_handler);
    HAL_TIM_Base_Start_IT(&tim3_handler);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&tim3_handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        stm32f767_timer_handler();
    }
}
