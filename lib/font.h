#ifndef __FONT_H_
#define __FONT_H_

#include <stdint.h>

typedef struct {
        unsigned char size;
        unsigned char height;
        unsigned char width;
        unsigned char *font_table;
} font_t;

extern const font_t ascii_fonts[4];

extern const uint8_t asc2_1206[95][12];
extern const uint8_t asc2_1608[95][16];
extern const uint8_t asc2_2412[95][48];
extern const uint8_t asc2_3216[95][128];

#endif
