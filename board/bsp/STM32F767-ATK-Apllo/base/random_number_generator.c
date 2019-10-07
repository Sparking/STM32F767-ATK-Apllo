#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_rng.h>
#include "rng.h"

static RNG_HandleTypeDef rng_handler;

void rng_init(void)
{
    rng_handler.Instance = RNG;
    HAL_RNG_Init(&rng_handler);
}

unsigned char rng_ready(void)
{
    return __HAL_RNG_GET_FLAG(&rng_handler, RNG_FLAG_DRDY) != RESET;
}

unsigned int rng_get_random(void)
{
    return HAL_RNG_GetRandomNumber(&rng_handler);
}
