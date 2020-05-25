#ifndef __STM32F767_ATK_APLLO_PCF8574_H__
#define __STM32F767_ATK_APLLO_PCF8574_H__

extern void stm32f767_atk_apllo_pcf8574_init(void);

extern unsigned char stm32f767_atk_apllo_pcf8574_read(void);

extern void stm32f767_atk_apllo_pcf8574_write(const unsigned char data);

extern void stm32f767_atk_apllo_pcf8574_setbit(const unsigned char bit, const unsigned char v);

extern unsigned char stm32f767_atk_apllo_pcf8574_getbit(const unsigned char bit);

#endif /* __STM32F767_ATK_APLLO_PCF8574_H__ */
