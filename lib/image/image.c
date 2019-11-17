#include <math.h>
#include <stdint.h>
#include <string.h>
#include "image/image.h"

int image_nearest_interp(const struct image *src_img, unsigned char *new_pixel,
        const float x, const float y)
{
    short row, column;

    row = (short)(y + 0.5f);
    column = (short)(x + 0.5f);

    if (row < 0 || row >= src_img->height || column < 0 || column >= src_img->width)
        return -1;

    *new_pixel = src_img->data[row * src_img->width + column];

    return 0;
}

int image_bilinear_interp(const struct image *src_img, unsigned char *new_pixel,
        const float x, const float y)
{
    const unsigned char *pixel[4];
    uint32_t x_scale, y_scale, x_fill_scale;

    if (x < 0 || (unsigned short)(x + 1) >= src_img->width
            || y < 0 || (unsigned short)(y + 1) >= src_img->height)
        return -1;

    pixel[0] = &src_img->data[((short)y) * src_img->width + ((short)x)];
    pixel[1] = pixel[0] + 1;
    pixel[2] = pixel[0] + src_img->width;
    pixel[3] = pixel[2] + 1;
    x_scale = ((uint32_t)(x * 256.0f)) & 255;
    y_scale = ((uint32_t)(y * 256.0f)) & 255;
    x_fill_scale = 256 - x_scale;
    *new_pixel = ((256 - y_scale) * (x_fill_scale * pixel[0][0] + x_scale * pixel[1][0])
        + y_scale * (x_fill_scale * pixel[2][0] + x_scale * pixel[3][0])) >> 16;

    return 0;
}

int image_bicubic_interp(const struct image *src_img, unsigned char *new_pixel,
        const float x, const float y)
{
    short wu[4], wv[4], g[4];
    unsigned short i, offset;
    int32_t u_scale, v_scale, diff;
    const unsigned char *pixel[17];
    const short a_16x = (short)(-0.5f * 16.0f);

    if (x < 1 || (unsigned short)(((short)x) + 2) >= src_img->width
            || y < 1 || (unsigned short)((short)(y) + 2) >= src_img->height)
        return -1;

    pixel[16] = src_img->data + (((short)y) - 1) * src_img->width + ((short)x) - 1;
    for (offset = 0; offset < 16;) {
        pixel[offset++] = pixel[16];
        pixel[16] += src_img->width;
        for (i = 1; i < 4; ++i, ++offset)
            pixel[offset] = pixel[offset - 1] + 1;
    }

    u_scale = ((int32_t)(x * 128.0f)) & 127;
    v_scale = ((int32_t)(y * 128.0f)) & 127;
    diff = 128 + u_scale;
    wu[0] = (a_16x * (-8388608 + (131072 - (640 - diff) * diff) * diff)) >> 4;
    diff = u_scale;
    wu[1] = 2097152 - ((((a_16x + 48) << 7) - (a_16x + 32) * diff) * diff * diff >> 4);
    diff = 128 - u_scale;
    wu[2] = 2097152 - ((((a_16x + 48) << 7) - (a_16x + 32) * diff) * diff * diff >> 4);
    diff = 256 - u_scale;
    wu[3] = (a_16x * (-8388608 + (131072 - (640 - diff) * diff) * diff)) >> 4;
    diff = 128 + v_scale;
    wv[0] = (a_16x * (-8388608 + (131072 - (640 - diff) * diff) * diff)) >> 4;
    diff = v_scale;
    wv[1] = 2097152 - ((((a_16x + 48) << 7) - (a_16x + 32) * diff) * diff * diff >> 4);
    diff = 128 - v_scale;
    wv[2] = 2097152 - ((((a_16x + 48) << 7) - (a_16x + 32) * diff) * diff * diff >> 4);
    diff = 256 - v_scale;
    wv[3] = (a_16x * (-8388608 + (131072 - (640 - diff) * diff) * diff)) >> 4;
    for (i = 0; i < 4; ++i) {
        g[i] = (wv[0] * pixel[i][0] + wv[1] * pixel[i + 4][0]
            + wv[2] * pixel[i + 8][0] + wv[3] * pixel[i + 12][0]) >> 21;
    }

    g[0] = (g[0] * wu[0] + g[1] * wu[1] + g[2] * wu[2] + g[3] * wu[3]) >> 21;
    if (g[0] >= 255) {
        g[0] = 255;
    } else if (g[0] <= 0) {
        g[0] = 0;
    }
    *new_pixel = (unsigned char)g[0];

    return 0;
}

// 耗时
int image_filter_median(const struct image *img, struct image *dst_img, const unsigned int m,
        const unsigned int n)
{
    unsigned int j_offset;
    unsigned int offset;
    unsigned int b_off[2];
    unsigned short i, j;
    unsigned short a, b, index;
    unsigned char counts_right;
    unsigned char counts_mid;
    unsigned char counts_left;
    unsigned char mid_index, f;
    unsigned char counts[256];
    unsigned char img_data[2];
    const unsigned char size = m * n;
    const unsigned char half_m = m >> 1;
    const unsigned char half_n = n >> 1;
    const unsigned char threshold = size >> 1;

    if (m >= img->height || n >= img->width || m > 15 || n > 15)
        return -1;

    if (size == 1)
        return 0;

    for (j = 0; j < img->height - m; ++j) {
        j_offset = j * img->width;
        counts_mid = 0;
        counts_left = 0;
        counts_right = 0;
        memset(counts, 0, sizeof(counts));

        /* 1. 更新直方图 */;
        for (b = j, offset = j_offset; b < j + m; ++b, offset += img->width) {
            for (a = 0; a < n; ++a)
                ++counts[img->data[offset + a]];
        }

        b = 0;
        mid_index = 0;
        for (f = 0, index = 0; index < 256; ++index) {
            b += counts[index];
            if (b < threshold) {
                counts_left = b;
            } else {
                if (f == 0) {
                    f = 1;
                    mid_index = index;
                    counts_mid = counts[index];
                } else {
                    counts_right += counts[index];
                }
            }
        }

        offset = dst_img->width * (j + half_m) + half_n;
        dst_img->data[offset] = mid_index;

        /* 2. 滑动窗口统计 */
        for (i = 1; i < img->width - n; ++i) {
            b_off[0] = j_offset + i - 1;
            b_off[1] = j_offset + i + n - 1;

            /* 2.1 重新统计直方图 */
            for (b = 0; b < m; ++b, b_off[0] += img->width, b_off[1] += img->width) {
                img_data[0] = img->data[b_off[0]];
                img_data[1] = img->data[b_off[1]];
                --counts[img_data[0]];
                ++counts[img_data[1]];
                if (img_data[0] > mid_index) {
                    --counts_right;
                } else if (img_data[0] < mid_index) {
                    --counts_left;
                } else {
                    --counts_mid;
                }

                if (img_data[1] > mid_index) {
                    ++counts_right;
                } else if (img_data[1] < mid_index) {
                    ++counts_left;
                } else {
                    ++counts_mid;
                }
            }

            /* 2.2 重新调整中值的游标 */
            if (counts_left >= threshold) {
                while (mid_index > 0) {
                    counts_right += counts[mid_index];
                    --mid_index;
                    counts_mid = counts[mid_index];
                    counts_left -= counts[mid_index];
                    if (counts_left < threshold)
                        break;
                }
            } else if (counts_mid + counts_left + counts_right >= threshold && counts_mid + counts_left < threshold) {
                while (mid_index < 255) {
                    counts_left += counts[mid_index];
                    ++mid_index;
                    counts_mid = counts[mid_index];
                    counts_right -= counts[mid_index];
                    if (counts_mid + counts_left >= threshold)
                        break;
                }
            }
            dst_img->data[offset + i] = mid_index;
        }
    }

    return 0;
}
