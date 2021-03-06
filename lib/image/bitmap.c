﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image/bitmap.h"

static uint8_t bmp_line_dummy_bytes(const uint32_t length, const uint32_t bit_count)
{
    return ((4 - (length * (bit_count >> 3))) & 3) & 3;
}

static void bmp_init_256_color_table(struct bmp_color_table pbct[256])
{
    for (uint16_t i = 0; i < 256; ++i) {
        pbct[i].blue = pbct[i].green = pbct[i].red = (uint8_t)i;
        pbct[i].reserved = 0;
    }
}

/**
 * @brief bmp_file_count_size_image 计算位图数据+调色板的实际大小
 * @param width 一行数据的长度
 * @param height 数据的行数
 * @param bit_count 每个数据的长度(bit)
 */
static uint32_t bmp_file_count_size_image(const uint32_t width, const uint32_t height, const uint32_t bit_count)
{
    const uint32_t dummy_line_bytes = bmp_line_dummy_bytes(width, bit_count);
    uint32_t bmp_size_image;

    switch (bit_count) {
    case 1:
        bmp_size_image = (sizeof(struct bmp_color_table) << 1) + ((width >> 3) + dummy_line_bytes) * height;
        break;
    case 4:
        bmp_size_image = (sizeof(struct bmp_color_table) << 4) + ((width >> 1) + dummy_line_bytes) * height;
        break;
    case 8:
        bmp_size_image = (sizeof(struct bmp_color_table) << 8) + (width + dummy_line_bytes) * height;
        break;
    default:
        bmp_size_image = (width * (bit_count >> 3) + dummy_line_bytes) * height;
        break;
    }

    return bmp_size_image;
}

/**
 * @brief bmp_file_count_size 计算位图文件的实际大小
 * @param width 一行数据的长度
 * @param height 数据的行数
 * @param bit_count 每个数据的长度(bit)
 */
static uint32_t bmp_file_count_size(const uint32_t width, const uint32_t height, const uint32_t bit_count)
{
    uint32_t bmp_file_size;

    bmp_file_size = sizeof(struct bmp_header) + sizeof(struct bmp_info_header)
        + bmp_file_count_size_image(width, height, bit_count);

    return bmp_file_size;
}

void bitmap_image_release(struct bitmap_image *img)
{
    if (img != NULL) {
        if (img->bct != NULL) {
            free(img->bct);
            img->bct = NULL;
        }

        if (img->data != NULL) {
            free(img->data);
            img->data = NULL;
        }
        free(img);
    }
}

struct bitmap_image *bitmap_image_create(const int32_t height, const int32_t width, const uint32_t bit_counts, const uint8_t gray)
{
    struct bitmap_image *image;
    const uint8_t data_size = bit_counts >> 3;

    image = (struct bitmap_image *)malloc(sizeof(struct bitmap_image));
    if (image == NULL)
        return NULL;

    memset(image, 0, sizeof(struct bitmap_image));
    image->bh.bf_type = BMP_FILE_TYPE;
    image->bh.bf_size = bmp_file_count_size(width, height, bit_counts);
    image->bh.bf_data_offset = sizeof(struct bmp_header) + sizeof(struct bmp_info_header);
    image->bih.bi_size = sizeof(struct bmp_info_header);
    image->bih.bi_height = height;
    image->bih.bi_width = width;
    image->bih.bi_planes = 1;
    image->bih.bi_bit_count = bit_counts;
    image->bih.bi_size_image = bmp_file_count_size_image(width, height, bit_counts);
    image->dummy_line_bytes = bmp_line_dummy_bytes(width, bit_counts);
    image->actual_height = height < 0 ? -height : height;
    image->data = (unsigned char *)malloc(image->actual_height * image->bih.bi_width * data_size);
    if (image->data == NULL) {
        free(image);
        return NULL;
    }

    image->bct = NULL;
    if (image->bih.bi_bit_count == 8) {
        image->bct = (struct bmp_color_table *)malloc(sizeof(struct bmp_color_table) << 8);
        if (image->bct == NULL) {
            free(image->data);
            free(image);
            return NULL;
        }

        bmp_init_256_color_table(image->bct);
        image->bh.bf_data_offset += sizeof(struct bmp_color_table) << 8;
    }
    memset(image->data, gray, image->actual_height * image->bih.bi_width * data_size);

    return image;
}

struct bitmap_image *bitmap_image_open(const char *filename)
{
    int ret;
    FILE *fp;
    struct bitmap_image *image;
    uint32_t line_size;
    int32_t y;
    uint8_t data_size;
    unsigned char *pdata;

    if (filename == NULL)
        return NULL;

    fp = fopen(filename, "rb");
    if (fp == NULL)
        return NULL;

    ret = -1;
    image = (struct bitmap_image *)malloc(sizeof(struct bitmap_image));
    if (image == NULL)
        goto release_resource;

    memset(image, 0, sizeof(struct bitmap_image));
    if (fread(&image->bh, sizeof(image->bh), 1, fp) != 1)
        goto release_resource;

    if (image->bh.bf_type != BMP_FILE_TYPE)
        goto release_resource;

    if (fread(&image->bih, sizeof(image->bih), 1, fp) != 1)
        goto release_resource;

    data_size = image->bih.bi_bit_count >> 3;
    line_size = image->bih.bi_width * data_size;
    image->actual_height = image->bih.bi_height;
    if (image->actual_height < 0)
        image->actual_height = -image->actual_height;
    image->dummy_line_bytes = bmp_line_dummy_bytes(image->bih.bi_width, image->bih.bi_bit_count);
    image->bct = NULL;
    image->data = (unsigned char *)malloc(image->actual_height * line_size);
    if (image->data == NULL)
        goto release_resource;

    if (image->bih.bi_bit_count == 8) {
        image->bct = (struct bmp_color_table *)malloc(sizeof(struct bmp_color_table) << 8);
        if (image->bct == NULL)
            goto release_resource;

        if (fread(image->bct, sizeof(struct bmp_color_table) << 8, 1, fp) != 1)
            goto release_resource;
    }

    for (y = 0, pdata = image->data; y < image->actual_height; ++y, pdata += line_size) {
        if (fread(pdata, line_size, 1, fp) != 1)
            goto release_resource;

        if (data_size == 4) {
            unsigned int x;

            for (x = 3; x < line_size; x += data_size)
                pdata[x] = 0xFF;
        }

        if (image->dummy_line_bytes)
            fseek(fp, image->dummy_line_bytes, SEEK_CUR);
    }

    ret = 0;
release_resource:
    if (ret == -1 && image != NULL) {
        if (image->data != NULL)
            free(image->data);

        if (image->bct != NULL)
            free(image->bct);

        free(image);
        image = NULL;
    }
    fclose(fp);

    return image;
}

int bitmap_image_save(const char *filename, const struct bitmap_image *image)
{
    int32_t y;
    FILE *fp;
    uint32_t data_size;
    uint32_t line_size;
    unsigned char *pdata;
    char dummy_bytes = 0;

    if (filename == NULL || image == NULL)
        return -1;

    fp = fopen(filename, "wb");
    if (fp == NULL)
        return -1;

    data_size = image->bih.bi_bit_count >> 3;
    line_size = image->bih.bi_width * data_size;
    fwrite((void *)&image->bh, sizeof(image->bh), 1, fp);
    fwrite((void *)&image->bih, sizeof(image->bih), 1, fp);
    if (image->bih.bi_bit_count == 8)
        fwrite((void *)image->bct, sizeof(struct bmp_color_table) << 8, 1, fp);

    for (y = 0, pdata = image->data; y < image->actual_height; ++y, pdata += line_size) {
        fwrite(pdata, line_size, 1, fp);
        if (image->dummy_line_bytes)
            fwrite(&dummy_bytes, 1, image->dummy_line_bytes, fp);
    }
    fflush(fp);
    fclose(fp);

    return 0;
}

static unsigned char image_rgb2gray(const unsigned char red, const unsigned char green, const unsigned char blue)
{
    //return (unsigned char)(0.114f * blue + 0.578f * green + 0.299f * red);
    return (unsigned char)((38 * (unsigned short)red + 75 * (unsigned short)green + 15 * (unsigned short)blue) >> 7);
}

struct image *bitmap_convert_image(const struct bitmap_image *bmp)
{
    struct image *img;
    unsigned int sz;
    unsigned int i, j, offset[2];
    int pixel_size, pixel_index;

    if (bmp == NULL)
        return NULL;

    sz = bmp->bih.bi_width * bmp->actual_height;
    img = (struct image *)malloc(sizeof(*img) + sz);
    if (img == NULL)
        return NULL;

    img->height = bmp->actual_height;
    img->width = bmp->bih.bi_width;
    pixel_size = bmp->bih.bi_bit_count >> 3;
    switch (bmp->bih.bi_bit_count) {
    case 8:
        if (bmp->bih.bi_height < 0) {
            for (i = 0; i < sz; ++i) {
                pixel_index = bmp->data[i];
                img->data[i] = image_rgb2gray(bmp->bct[pixel_index].red, bmp->bct[pixel_index].green, bmp->bct[pixel_index].blue);
            }
        } else {
            for (j = img->height, offset[0] = 0, offset[1] = sz - img->width; j-- > 0;) {
                for (i = 0; i < img->width; ++i) {
                    pixel_index = bmp->data[offset[1] + i];
                    img->data[offset[0] + i] = image_rgb2gray(bmp->bct[pixel_index].red, bmp->bct[pixel_index].green, bmp->bct[pixel_index].blue);
                }
                offset[0] += img->width;
                offset[1] -= img->width;
            }
        }
        break;
    case 24:
    case 32:
        if (bmp->bih.bi_height < 0) {
            for (i = 0; i < sz; ++i) {
                pixel_index = i * pixel_size;
                img->data[i] = image_rgb2gray(bmp->data[pixel_index + 2], bmp->data[pixel_index + 1], bmp->data[pixel_index]);
            }
        } else {
            for (j = img->height, offset[0] = 0, offset[1] = (sz - img->width) * pixel_size; j-- > 0;) {
                for (i = 0; i < img->width; ++i) {
                    pixel_index = offset[1] + i * pixel_size;
                    img->data[offset[0] + i] = image_rgb2gray(bmp->data[pixel_index + 2], bmp->data[pixel_index + 1], bmp->data[pixel_index]);
                }
                offset[0] += img->width;
                offset[1] -= img->width * pixel_size;
            }
        }
        break;
    default:
        free(img);
        return NULL;
    }

    return img;
}

struct bitmap_image *image_convert_bitmap(const struct image *img)
{
    struct bitmap_image *bmp;

    if (img == NULL)
        return NULL;

    bmp = bitmap_image_create(-((int)img->height), img->width, 8, 0);
    if (bmp == NULL)
        return NULL;

    memcpy(bmp->data, img->data, img->height * img->width);

    return bmp;
}

