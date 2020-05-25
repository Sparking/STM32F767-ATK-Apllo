#ifndef __LCD_CONFIG_H_
#define __LCD_CONFIG_H_

#include <string.h>
#include "stm32f7xx_hal.h"
#include "common/maths.h"
#include "font.h"

#define LCD_BG_COLOR  0xFCC3U

struct __ltdc_devicetypedef {
        uint8_t  hbp;      /* 水平后沿宽度 */
        uint8_t  vbp;      /* 垂直后沿宽度 */
        uint8_t  hfp;      /* 水平前沿宽度 */
        uint8_t  vfp;      /* 垂直前沿宽度 */
        uint16_t hsync;    /* 水平同步宽度 */
        uint16_t vsync;    /* 垂直同步宽度 */
        uint16_t active_width; /* 有效宽度 */
        uint16_t active_height;/* 有效高度 */
};

extern const struct __ltdc_devicetypedef __ltdc_lcd_device;

typedef enum {
        LCD_LAYER_1 = 0U,  /* 第一层显示层 */
        LCD_LAYER_2 = 1U   /* 第二层显示层 */
} lcd_layer_e;

typedef enum {
        LCD_DIRECTION_H = 0U, /* 横屏显示 */
        LCD_DIRECTION_V = 1U  /* 竖屏显示 */
} lcd_dir_e;

typedef struct {
        lcd_layer_e Layer;  /* 显示层设置 */
        /* 横竖屏显示设置 */
        lcd_dir_e Direction;

        /*
         * 不论是横屏显示还是竖屏显示，X轴的方向都是横向朝右
         * Y轴的方向都是竖直朝下。
         */
        /* X轴(水平轴,方向朝右), Y轴(竖直轴,方向朝下) */
        struct point start_pos;
        struct point end_pos;
} lcd_window_t;

typedef struct {
        unsigned int Color; /* 颜色 */
        lcd_layer_e Layer;  /* 显示层设置 */
        lcd_dir_e Direction; /* 横竖屏设置 */
        struct point pos; /* 点的位置 */
} lcd_point_config_t; /* 显示位置初始化结构体 */

typedef struct {
        lcd_point_config_t point_config; /* 指定了位置及显示的颜色 */
        struct {
                const font_t *font; /* 使用的字体 */
                unsigned int backclr; /* 字体的背景色 */
                enum {
                        FONT_DISPLAY_WITH_BACK_COLOR    = 0U, /* 打印字体需要有背景色 */
                        FONT_DISPLAY_WITHOUT_BACK_COLOR = 1U  /* 字体显示无背景色 */
                } mode; /* 字体显示模式 */
        } font_config; /* 字体配置 */
} lcd_char_config_t;

/*
 * 像素格式
 *   000:ARGB8888
 *   001:RGB888
 *   010:RGB565
 *   011:ARGB1555
 *   100:ARGB4444
 *   101:L8(8 位 Luminance)
 *   110:AL44(4 位 Alpha,4 位 Luminance)
 *   111:AL88(8 位 Alpha,8 位 Luminance)
 */
#define LTDC_PIXEL_FORMAT 0x00000002 /* LCD像素格式 */
#define LTDC_LAYER_2_ENABLE 0U  /* 禁止LTDC第二层 */

/* 显示缓冲配置 */
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
#define LTDC_LAYERS_NUM 2U      /* 设置显存位置为两片缓冲区 */
#else
#define LTDC_LAYERS_NUM 1U      /* 设置显存位置为一片缓冲区 */
#endif

/* LCD显存设置 */
#define LTDC_PIXEL_SIZE 16         /* LCD像素16 bits */
typedef uint16_t lcd_pixel_t;      /* LCD像素类型定义 */
#define LCD_FRAME_BUFF_ADDRESS 0xC0000000U /* LCD显存地址 */

/*
 * 函数stm32f767_atk_apllo_lcd_init 初始化LCD
 */
void stm32f767_atk_apllo_lcd_init(void);

/*
 * 函数stm32f767_atk_apllo_lcd_draw_point LCD中的画点函数
 */
void stm32f767_atk_apllo_lcd_draw_point(lcd_point_config_t *hlp);

/*
 * stm32f767_atk_apllo_lcd_puts LCD打印字符串
 * 参数hld:
 *      类型为lcd_char_config_t *
 *  String(表示字符串),函数LCD_Puts只打印成员String而不打印Charactor
 */
unsigned int stm32f767_atk_apllo_lcd_puts(lcd_char_config_t *hld, char *string);

/*
 * stm32f767_atk_apllo_lcd_putc LCD打印字符
 */
unsigned int stm32f767_atk_apllo_lcd_putc(lcd_char_config_t *hld, char ch);

/*
 * stm32f767_atk_apllo_lcd_fill_window LCD窗口填充函数
 * 以一种颜色来填充指定的窗口
 */
void stm32f767_atk_apllo_lcd_fill_window(const lcd_window_t *hlfw, unsigned int color);

/*
 * stm32f767_atk_apllo_lcd_clear LCD清屏函数
 */
void stm32f767_atk_apllo_lcd_clear(unsigned int color);

/*
 * stm32f767_atk_apllo_lcd__blockcopy LCD显示区域拷贝函数
 * 作用:
 *  将某一区域的内容复制到另一个区域中
 */
void stm32f767_atk_apllo_lcd__blockcopy(const lcd_window_t *src,
                   const lcd_window_t *dist);

/*
 * stm32f767_atk_apllo_lcd_flush
 * 作用:
 *  刷新DMA2D的输出，防止输出结果被DMA2D填充
 */
 void stm32f767_atk_apllo_lcd_flush(void);
 
#endif
