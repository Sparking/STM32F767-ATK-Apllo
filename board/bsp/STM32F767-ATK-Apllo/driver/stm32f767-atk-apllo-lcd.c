#include <stdint.h>
#include "stm32f767-atk-apllo-lcd.h"

const struct __ltdc_devicetypedef __ltdc_lcd_device = { /* 方便统一修改 */
    .hsync    = 1,
    .vsync    = 1,
    .hbp      = 40,
    .vbp      = 8,
    .hfp      = 5,
    .vfp      = 8,
    .active_width = 480,
    .active_height = 272
};

#define __lcd_frame_buff ((lcd_pixel_t *)(LCD_FRAME_BUFF_ADDRESS));

static char LTDC_Clk_Set(const unsigned int pllsain, const unsigned int pllsair, const unsigned int pllsaidivr)
{
    RCC_PeriphCLKInitTypeDef PeriphClkIniture;

    //LTDC输出像素时钟，需要根据自己所使用的LCD数据手册来配置！
    PeriphClkIniture.PeriphClockSelection = RCC_PERIPHCLK_LTDC;   //LTDC时钟
    PeriphClkIniture.PLLSAI.PLLSAIN = pllsain;
    PeriphClkIniture.PLLSAI.PLLSAIR = pllsair;
    PeriphClkIniture.PLLSAIDivR = pllsaidivr;

    return (HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture) == HAL_OK); //配置像素时钟，这里配置为时钟为18.75MHZ
}

#if 1
void stm32f767_atk_apllo_lcd_init(void)
{
    uint16_t ltdc_hconfig_value, ltdc_vconfig_value;

    LTDC_Clk_Set(288, 4, RCC_PLLSAIDIVR_8);
    HAL_LTDC_MspInit(NULL);
/**** 同步信号和时钟极性配置 ****/
/* LTDC->GCR 全局控制寄存器
 * HSPOL[31]:水平同步极性 0:低电平有效 1:高电平有效
 * VSPOL[30]:垂直同步极性 0:低电平有效 1:高电平有效
 * DEPOL[29]:数据使能极性 0:低电平有效 1:高电平有效
 * PCPOL[28]:像素时钟极性 0:低电平有效 1:高电平有效
 * DEN[16]:    抖动使能 0:禁止 1:使能
 * DRW[14:12]: 抖动红色宽度
 * DGW[10:8]:  抖动绿色宽度
 * DBW[6:4]:   抖动蓝色宽度
 * LTDCEN[0]: LCD-TFT控制器使能
 */
    LTDC->GCR = 0x00000001; /* 最后再使能,先配置同步信号和时钟极性 */
/**** LCD 同步时序配置 ****/
    /* LTDC同步周期大小配置 */
    ltdc_hconfig_value = __ltdc_lcd_device.hsync - 1;
    ltdc_vconfig_value = __ltdc_lcd_device.vsync - 1;
    LTDC->SSCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
    /* LTDC后沿配置 */
    ltdc_hconfig_value += __ltdc_lcd_device.hbp;
    ltdc_vconfig_value += __ltdc_lcd_device.vbp;
    LTDC->BPCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
    /* LTDC有效宽度配置 */
    ltdc_hconfig_value += __ltdc_lcd_device.active_width;
    ltdc_vconfig_value += __ltdc_lcd_device.active_height;
    LTDC->AWCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
    /* LTDC总宽度配置 */
    ltdc_hconfig_value += __ltdc_lcd_device.hfp;
    ltdc_vconfig_value += __ltdc_lcd_device.vfp;
    LTDC->TWCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
/**** 背景色配置 ****/
/* LTDC->BCCR 背景色控制寄存器
 * BCRED[23:16]:  背景红色值
 * BCGREEN[15:8]: 背景绿色值
 * BCBLUE[7:0]:   背景蓝色值
 */
    LTDC->BCCR = 0;
/**** 中断配置 ****/
    /* LTDC中断配置(全禁用) */
    LTDC->IER = 0;
    /* LTDC行中断位置配置 */
    LTDC->LIPCR = 0;

/************ LTDC层参数配置 ***************/
/**** LTDC窗口水平位置和垂直位置配置 ****/
/* LTDC_Layerx->WHPCR 层窗口水平位置配置
 * WHSPPOS[27:16]: 窗口水平结束位置
 * WHSTPOS[11:0]:  窗口水平起始位置
 */
    ltdc_hconfig_value = __ltdc_lcd_device.hsync + __ltdc_lcd_device.hbp;
    LTDC_Layer1->WHPCR = ((ltdc_hconfig_value + __ltdc_lcd_device.active_width - 1) << 16)
            | ltdc_hconfig_value;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
    LTDC_Layer2->WHPCR = ((ltdc_hconfig_value + __ltdc_lcd_device.active_width - 1) << 16)
            | ltdc_hconfig_value;
#endif
/* LTDC_Layerx->WVPCR 层窗口垂直位置配置
 * WCSPPOS[26:16]: 窗口垂直结束位置
 * WVSTPOS[10:0]:  窗口垂直起始位置
 */
    ltdc_vconfig_value = __ltdc_lcd_device.vsync + __ltdc_lcd_device.vbp;
    LTDC_Layer1->WVPCR = ((ltdc_vconfig_value + __ltdc_lcd_device.active_height - 1) << 16)
            | ltdc_vconfig_value;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
    LTDC_Layer2->WVPCR = ((ltdc_vconfig_value + __ltdc_lcd_device.active_height - 1) << 16)
            | ltdc_vconfig_value;
#endif
/**** LTDC层像素输入格式 ****/
/* LTDC_Layerx->PFCR
 * PF[2:0]:像素格式
 */
    LTDC_Layer1->PFCR  = LTDC_PIXEL_FORMAT;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
    LTDC_Layer2->PFCR  = LTDC_PIXEL_FORMAT;
#endif
/**** LTDC颜色帧配置 ****/
/* LTDC_Layerx->CFBAR 颜色帧缓冲区地址
 * CFBADD[31:0]:颜色帧缓冲区起始地址 (Color Frame Buffer Start Address)
 *   这些位定义颜色帧缓冲区的起始地址。
 *
 * LTDC_Layerx->CFBLR 颜色帧缓冲区长度
 * CFBP[28:16]: 颜色帧缓冲区间距(以字节为单位)
 *   这些位定义从像素某行的起始处到下一行的起始处的增量(以字节为单位)。
 * CFBLL[12:0]: 颜色帧缓冲区行长
 *   这些位定义一行像素的长度(以字节为单位)+ 3。
 *   行长的计算方法为:有效宽度 x 每像素的字节数 + 3。
 *
 * LTDC_Layerx->CFBLNR 颜色帧缓冲区行数
 * CFBLNBR[10:0]:帧缓冲区行数
 *   这些位用来定义缓冲区中的行数,行数跟有效高度对应。
 */
    LTDC_Layer1->CFBAR  = LCD_FRAME_BUFF_ADDRESS;       /* LCD显存位置 */
    /* 480 * 2(RGB565) => 0x03C0 */
    ltdc_hconfig_value  = __ltdc_lcd_device.active_width << 1;
    LTDC_Layer1->CFBLR  = (ltdc_hconfig_value << 16) | (ltdc_hconfig_value + 3); /* 0x03C003C3 */
    LTDC_Layer1->CFBLNR = __ltdc_lcd_device.active_height;     /* 272 */
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
    LTDC_Layer2->CFBAR  = LCD_FRAME_BUFF_ADDRESS 
            + __ltdc_lcd_device.active_height * __ltdc_lcd_device.active_width;
    ltdc_hconfig_value  = __ltdc_lcd_device.active_width << 1;
    LTDC_Layer2->CFBLR  = (ltdc_hconfig_value << 16) | (ltdc_hconfig_value + 3);
    LTDC_Layer2->CFBLNR = __ltdc_lcd_device.active_height;
#endif
/**** CLUT加载RGB值及其 ****/
/* LTDC_Layerx->CLUTWR CLUT写寄存器
 * CLUTADD[31:24]: CLUT地址
 *   这些位配置每个RGB值的CLUT地址(CLUT内的颜色位置)
 * RED[23:16]: 红色值
 *   这些位配置红色值
 * GREEN[15:8]: 绿色值
 *   这些位配置绿色值
 * BLUE[7:0]: 蓝色值
 *   这些位配置蓝色值
 * CLUT可通过LTDC_LxCR寄存器使能和禁止。
 * CLUT仅对L8、AL44和AL88像素格式有意义。
 */
/*       LTDC_Layer1->CLUTWR = 0; */
/*       LTDC_Layer2->CLUTWR = 0; */
/**** 默认颜色和混合系数配置 ****/
/* LTDC_Layerx->DCCR 默认颜色配置寄存器
 * DCALPHA[31:24]:默认Alpha值
 *   这些位配置默认alpha值
 * DCRED[23:16]:默认颜色红色
 *   这些位配置默认红色值
 * DCGREEN[15:8]:默认颜色绿色
 *   这些位配置默认绿色值
 * DCBLUE[7:0]:默认颜色蓝色
 *   这些位配置默认蓝色值
 *
 * LTDC_Layerx->BFCR 混合系数配置(配置详情请参考 STM32F7xx中文参考手册P498)
 *     LTDC_Layerx->CACR用于设置常数Alpha的值
 * 通用混合公式为:BC = BF1 x C + BF2 x Cs
 *  BC  = 混合后的颜色
 *  BF1 = 混合系数 1
 *  C   = 当前层颜色
 *  BF2 = 混合系数 2
 *  Cs  = 底层混合后的颜色
 * BF1[10:8]:取值如下
 *  100: 常数Alpha
 *  110: 像素Alpha*常数Alpha
 * BF2[2:0]:取值如下
 *  101: 1——常数Alpha
 *  111: 1——像素Alpha*常数Alpha
 */
    LTDC_Layer1->DCCR = 0x00000000;
    LTDC_Layer1->CACR = 0x000000FF; /* 常数Alpha = 配置值/255 */
    LTDC_Layer1->BFCR = 0x00000203;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
    LTDC_Layer2->DCCR = 0x00000000;
    LTDC_Layer2->CACR = 0x0000007F;
    LTDC_Layer2->BFCR = 0x00000203;
#endif
 /**** 使能层数 ****/
 /* LTDC_Layerx->CR 控制寄存器
  * CLUTEN[4]:查色表使能
  *   0:禁止查色表
  *   1:使能查色表
  *   CLUT 仅对 L8、AL44 和 AL88 像素格式有意义。请参见第 479 页上的查色表 (CLUT)。
  * COLKEN[1]:色键使能
  *   0:禁止色键
  *   1:使能色键
  * LEN[0]:层使能
  *   0:禁止层
  *   1:使能层
  */
    LTDC_Layer1->CR = 0x1;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
    LTDC_Layer2->CR = 0x1;
#endif
/**** 将影子寄存器重装载到活动寄存器 ****/
    LTDC->SRCR |= 0x1; /* 立即重载 */
/**** 使能LCD-TFT控制器 ****/
    LTDC->GCR  |= 0x1;
}
#else
static LTDC_HandleTypeDef  LTDC_Handler;

static void LTDC_Layer_Parameter_Config(void *h, unsigned int bufaddr, unsigned char layerx,
        unsigned char pixformat, unsigned char alpha, unsigned char alpha0,
        unsigned char bfac1, unsigned char bfac2, unsigned int bkcolor)
{
    LTDC_LayerCfgTypeDef pLayerCfg;

    pLayerCfg.WindowX0=0;                       //窗口起始X坐标
    pLayerCfg.WindowY0=0;                       //窗口起始Y坐标
    pLayerCfg.WindowX1=__ltdc_lcd_device.active_width;          //窗口终止X坐标
    pLayerCfg.WindowY1=__ltdc_lcd_device.active_height;         //窗口终止Y坐标
    pLayerCfg.PixelFormat=pixformat;            //像素格式
    pLayerCfg.Alpha=alpha;                      //Alpha值设置，0~255,255为完全不透明
    pLayerCfg.Alpha0=alpha0;                    //默认Alpha值
    pLayerCfg.BlendingFactor1=(unsigned int)bfac1<<8;    //设置层混合系数
    pLayerCfg.BlendingFactor2=(unsigned int)bfac2<<8;    //设置层混合系数
    pLayerCfg.FBStartAdress=bufaddr;            //设置层颜色帧缓存起始地址
    pLayerCfg.ImageWidth=__ltdc_lcd_device.active_width;        //设置颜色帧缓冲区的宽度    
    pLayerCfg.ImageHeight=__ltdc_lcd_device.active_height;      //设置颜色帧缓冲区的高度
    pLayerCfg.Backcolor.Red=(unsigned char)(bkcolor&0X00FF0000)>>16;   //背景颜色红色部分
    pLayerCfg.Backcolor.Green=(unsigned char)(bkcolor&0X0000FF00)>>8;  //背景颜色绿色部分
    pLayerCfg.Backcolor.Blue=(unsigned char)bkcolor&0X000000FF;        //背景颜色蓝色部分
    HAL_LTDC_ConfigLayer(h, &pLayerCfg,layerx);   //设置所选中的层
}

void stm32f767_atk_apllo_lcd_init(void)
{
    memset(&LTDC_Handler, 0, sizeof(LTDC_Handler));

    LTDC_Clk_Set(288, 4, RCC_PLLSAIDIVR_8);
    LTDC_Handler.Instance = LTDC;
    LTDC_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;            //水平同步极性
    LTDC_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;            //垂直同步极性
    LTDC_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;            //数据使能极性
    LTDC_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;           //像素时钟极性
    LTDC_Handler.Init.HorizontalSync = __ltdc_lcd_device.hsync - 1;             //水平同步宽度
    LTDC_Handler.Init.VerticalSync = __ltdc_lcd_device.vsync-1;               //垂直同步宽度
    LTDC_Handler.Init.AccumulatedHBP = __ltdc_lcd_device.hsync + __ltdc_lcd_device.hbp - 1; //水平同步后沿宽度
    LTDC_Handler.Init.AccumulatedVBP = __ltdc_lcd_device.vsync + __ltdc_lcd_device.vbp - 1; //垂直同步后沿高度
    LTDC_Handler.Init.AccumulatedActiveW = __ltdc_lcd_device.hsync + __ltdc_lcd_device.hbp + __ltdc_lcd_device.active_width - 1;//有效宽度
    LTDC_Handler.Init.AccumulatedActiveH = __ltdc_lcd_device.vsync + __ltdc_lcd_device.vbp + __ltdc_lcd_device.active_height - 1;//有效高度
    LTDC_Handler.Init.TotalWidth = __ltdc_lcd_device.hsync + __ltdc_lcd_device.hbp + __ltdc_lcd_device.active_width + __ltdc_lcd_device.hfp - 1;   //总宽度
    LTDC_Handler.Init.TotalHeigh = __ltdc_lcd_device.vsync + __ltdc_lcd_device.vbp + __ltdc_lcd_device.active_height + __ltdc_lcd_device.vfp - 1;  //总高度
    LTDC_Handler.Init.Backcolor.Red = 0;           //屏幕背景层红色部分
    LTDC_Handler.Init.Backcolor.Green = 0;         //屏幕背景层绿色部分
    LTDC_Handler.Init.Backcolor.Blue = 0;          //屏幕背景色蓝色部分
    HAL_LTDC_Init(&LTDC_Handler);

    LTDC_Layer_Parameter_Config(&LTDC_Handler, LCD_FRAME_BUFF_ADDRESS, 0, LTDC_PIXEL_FORMAT, 255, 0, 6, 7,0X000000);
    HAL_LTDC_SetWindowPosition(&LTDC_Handler, 0, 0, 0);
    HAL_LTDC_SetWindowSize(&LTDC_Handler, __ltdc_lcd_device.active_width, __ltdc_lcd_device.active_height, 0);//设置窗口大小
    stm32f767_atk_apllo_lcd_clear(0x12312312);
}
#endif

void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
    GPIO_InitTypeDef GPIO_Initure;
/****** GPIO 配置开始 ******/
/*
 * PB5: LCD_BL LCD背光
 */
    GPIO_Initure.Pin  = GPIO_PIN_5;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_LOW;
    GPIO_Initure.Alternate = 0;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
/*
 * PF10: LCD_DE
 */
    GPIO_Initure.Pin  = GPIO_PIN_10;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed= GPIO_SPEED_HIGH;
    GPIO_Initure.Alternate = GPIO_AF14_LTDC;
    __HAL_RCC_GPIOF_CLK_ENABLE();
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
/*
 * PG6: LCD_R[7]
 * PG7: LCD_CLK
 * PG11: LCD_B[3]
 */
    GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11;
    __HAL_RCC_GPIOG_CLK_ENABLE();
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);
/*
 * PH9: LCD_R[3]
 * PH10: LCD_R[4]
 * PH11: LCD_R[5]
 * PH12: LCD_R[6]
 * PH13: LCD_G[2]
 * PH14: LCD_G[3]
 * PH15: LCD_G[4]
 */
    GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12
        | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    __HAL_RCC_GPIOH_CLK_ENABLE();
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
/*
 * PI0: LCD_G[5]
 * PI1: LCD_G[6]
 * PI2: LCD_G[7]
 * PI4: LCD_B[4]
 * PI5: LCD_B[5]
 * PI6: LCD_B[6]
 * PI7: LCD_B[7]
 * PI9: LCD_VSYNC
 * PI10: LCD_HSYNC
 */
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4
        | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9
        | GPIO_PIN_10;
    __HAL_RCC_GPIOI_CLK_ENABLE();
    HAL_GPIO_Init(GPIOI, &GPIO_Initure);
/****** GPIO 配置完成 ******/
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_DMA2D_CLK_ENABLE();
}

static lcd_pixel_t *lcd_get_point_addr(const lcd_layer_e layer, const struct point pos, const lcd_dir_e dir)
{
    lcd_pixel_t *frame = __lcd_frame_buff;

    if (layer == LCD_LAYER_2)
        frame = (lcd_pixel_t *)((uint32_t)frame + __ltdc_lcd_device.active_height * __ltdc_lcd_device.active_width);

    return (dir == LCD_DIRECTION_V) ?
          &frame[__ltdc_lcd_device.active_width *
            (__ltdc_lcd_device.active_height - pos.x - 1) + pos.y] :
          &frame[pos.y * __ltdc_lcd_device.active_width + pos.x];
}

void stm32f767_atk_apllo_lcd_draw_point(lcd_point_config_t *hldp)
{
    *lcd_get_point_addr(hldp->Layer, hldp->pos, hldp->Direction) = hldp->Color;
}

unsigned int stm32f767_atk_apllo_lcd_putc(lcd_char_config_t *hld, char ch)
{
    unsigned char cs, i; /* charactor(width) size */
    unsigned char ch_index = ch - ' ';
    unsigned int temp;
    unsigned int x_pos = hld->point_config.pos.x;
    unsigned int y_pos = hld->point_config.pos.y;
    unsigned int bg_color = hld->font_config.backclr;
    unsigned int char_color = hld->point_config.Color;

    if (hld->font_config.font == NULL) {
        return x_pos;
    }
    for (cs = 0; cs < hld->font_config.font->width; cs ++) {
        temp = hld->font_config.font->font_table
            [ch_index * hld->font_config.font->width + cs];
        for (i = 0; i < 8; i++) {
            if (temp & 0x80) {
                hld->point_config.Color = char_color;
                stm32f767_atk_apllo_lcd_draw_point(&hld->point_config);
            } else if(hld->font_config.mode == FONT_DISPLAY_WITH_BACK_COLOR) {
                hld->point_config.Color = bg_color;
                stm32f767_atk_apllo_lcd_draw_point(&hld->point_config);
            }

            temp <<= 1;
            hld->point_config.pos.y++;
            if (hld->point_config.pos.y - y_pos == hld->font_config.font->height) {
                hld->point_config.pos.y = y_pos;
                hld->point_config.pos.x++;
            }
        }
    }
    temp = hld->point_config.pos.x;
    hld->point_config.pos.x = x_pos;
    hld->point_config.pos.y = y_pos;
    hld->point_config.Color = char_color;

    return temp;
}

unsigned int stm32f767_atk_apllo_lcd_puts(lcd_char_config_t *hld, char *string)
{
    unsigned int temp;
    unsigned int index;
    unsigned int X_pos_backup = hld->point_config.pos.x;

    for (index = 0; string[index] != '\0'; index ++) {
        if ((temp = stm32f767_atk_apllo_lcd_putc(hld, string[index])) == hld->point_config.pos.x)
            break;
        hld->point_config.pos.x = temp;
    }
    hld->point_config.pos.x = X_pos_backup;
    return index;
}

static void static_stm32f767_atk_apllo_lcd_convert_xy(const lcd_window_t *hlfw,
        int *converted_x, int *converted_y, unsigned int *lines, unsigned int *line_size)
{
    if (hlfw->Direction == LCD_DIRECTION_V) {
        *lines     = hlfw->end_pos.x - hlfw->start_pos.x;
        *line_size = hlfw->end_pos.y - hlfw->start_pos.y;
        *converted_x = hlfw->start_pos.y;
        *converted_y = __ltdc_lcd_device.active_height - hlfw->end_pos.x;
    }
    else {
        *lines     = hlfw->end_pos.y - hlfw->start_pos.y;
        *line_size = hlfw->end_pos.x - hlfw->start_pos.x;
        *converted_x = hlfw->start_pos.x;
        *converted_y = hlfw->start_pos.y;
    }
}

void stm32f767_atk_apllo_lcd_fill_window(const lcd_window_t *hlfw, unsigned int color)
{
    unsigned int lines, line_size;
    struct point pos;

    stm32f767_atk_apllo_lcd_flush();
    DMA2D->CR = (3 << 16); /* 设置DMA2D的模式 */
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT; /* 输出像素格式为RGB565 */
    DMA2D->OCOLR = color; /* 填充的颜色 */
    static_stm32f767_atk_apllo_lcd_convert_xy(hlfw, &pos.x, &pos.y, &lines, &line_size);
    DMA2D->OMAR = (unsigned int)lcd_get_point_addr(hlfw->Layer, pos,
        LCD_DIRECTION_H); /* 存储器地址 */
    DMA2D->OOR = __ltdc_lcd_device.active_width - line_size;
    DMA2D->NLR = lines + (line_size << 16);
    DMA2D->CR |= 0x0001;
}

void stm32f767_atk_apllo_lcd__blockcopy(const lcd_window_t *src, const lcd_window_t *dist)
{
    unsigned int lines, line_size;
    struct point src_pos, dist_pos;

    stm32f767_atk_apllo_lcd_flush();
    static_stm32f767_atk_apllo_lcd_convert_xy(src, &src_pos.x, &src_pos.y, &lines, &line_size);
    DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT;
    DMA2D->FGMAR = (unsigned int)lcd_get_point_addr(src->Layer, src_pos,
        LCD_DIRECTION_H); /* 存储器地址 */
    DMA2D->FGOR = __ltdc_lcd_device.active_width - line_size;
    DMA2D->OOR = __ltdc_lcd_device.active_width - line_size;
    DMA2D->NLR = lines + (line_size << 16);
    static_stm32f767_atk_apllo_lcd_convert_xy(dist, &dist_pos.x, &dist_pos.y, &lines, &line_size);
    DMA2D->OMAR = (unsigned int)lcd_get_point_addr(dist->Layer, dist_pos,
        LCD_DIRECTION_H); /* 存储器地址 */
    DMA2D->CR = 0x0001;
}

void stm32f767_atk_apllo_lcd_clear(unsigned int color)
{
    lcd_window_t LCD_Fill_Handler;

/* LCD填充区域 */
    LCD_Fill_Handler.Layer = LCD_LAYER_1; /* 显示层设为第一层 */
    LCD_Fill_Handler.Direction = LCD_DIRECTION_H; /* 横屏显示 */
    LCD_Fill_Handler.start_pos.x = 0; /* 开始位置X轴 */
    LCD_Fill_Handler.start_pos.y = 0; /* 开始位置Y轴 */
    LCD_Fill_Handler.end_pos.x = 480; /* 结束位置X轴 */
    LCD_Fill_Handler.end_pos.y = 272; /* 结束位置Y轴 */
    stm32f767_atk_apllo_lcd_fill_window(&LCD_Fill_Handler, color);  /* 开始填充 */
    stm32f767_atk_apllo_lcd_flush();
}

void stm32f767_atk_apllo_lcd_flush(void)
{
    unsigned int time = 0;

    while ((DMA2D->ISR & (1 << 1)) == 0) {   //等待传输完成
        time++;
        if (time > 0X1FFFFF)
            break;  //超时退出
    }

    DMA2D->IFCR |= 1 << 1; //清除传输完成标志
}
