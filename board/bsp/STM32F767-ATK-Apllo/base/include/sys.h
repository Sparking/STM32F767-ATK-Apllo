#ifndef __BASE_SYS_H_
#define __BASE_SYS_H_

#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx.h>
#include <stm32f7xx_hal.h>

extern void delay_us(unsigned int nus);
extern void delay_ms(unsigned int nms);
extern void delay_xms(unsigned int ms);

extern unsigned int get_systick_count(void);

extern void system_init(const unsigned int baudrate);

#endif /* __BASE_SYS_H_ */
