#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* 图像的原点为左上角 */
struct image {
    unsigned short height;  /* 图像的高度 */
    unsigned short width;   /* 图像的宽度 */
    unsigned char data[0];  /* 图像数据, 8位灰度图 */
};

extern int image_nearest_interp(const struct image *src_img, unsigned char *new_pixel,
        const float x, const float y);

extern int image_bilinear_interp(const struct image *src_img, unsigned char *new_pixel,
        const float x, const float y);

extern int image_bicubic_interp(const struct image *src_img, unsigned char *new_pixel,
        const float x, const float y);

extern int image_filter_median(const struct image *img, struct image *dst_img,
		const unsigned int m, const unsigned int n);

#ifdef __cplusplus
}
#endif

