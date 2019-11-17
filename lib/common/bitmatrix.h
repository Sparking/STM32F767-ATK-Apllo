﻿#pragma once

#ifdef __cpluscplus
extern "C" {
#endif

struct bitmatrix {
    unsigned short row;       /* 行数 */
    unsigned short column;    /* 列数 */

    unsigned short row_size;  /* 每一行的字节长度 */
    unsigned short size;      /* 总大小 */
    char matrix[0];
};

static inline unsigned short bitmatrix_size(const struct bitmatrix *matrix)
{
    return matrix->size + sizeof(struct bitmatrix);
}

/**
 * @brief bitmatrix_create_static 创建一个指定大小的静态二元矩阵
 * @param buff 静态内存区域
 * @param row 行数
 * @param column 列数
 * @param reset_bit 矩阵的全局复位值,0和1
 */
extern struct bitmatrix *bitmatrix_create_static(void *buff, const unsigned short row,
        const unsigned short column, const unsigned char reset_bit);

/**
 * @brief bitmatrix_set 设置二元矩阵某一位的值
 * @param row 行数
 * @param column 列数
 * @param bit 矩阵元素的设定值, 0和1
 * @return 返回被设定之前的元素值
 */
extern unsigned char bitmatrix_set(struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column, const unsigned char bit);

/**
 * @brief bitmatrix_xor 二元矩阵某一位进行异或
 * @param row 行数
 * @param column 列数
 * @param bit 和矩阵元素进行异或运算的值, 0和1
 * @return 返回异或运算之前的元素值
 */
extern unsigned char bitmatrix_xor(struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column, const unsigned char bit);

/**
 * @brief bitmatrix_flip 二元矩阵某一位进行翻转
 * @param row 行数
 * @param column 列数
 * @return 返回翻转之前的元素值
 */
extern unsigned char bitmatrix_flip(struct bitmatrix *matrix, const unsigned short row,
        const unsigned short column);

/**
 * @brief bitmatrix_get 获取二元矩阵某一位的值
 * @param row 行数
 * @param column 列数
 */
extern unsigned char bitmatrix_get(const struct bitmatrix *matrix,
        const unsigned short row, const unsigned short column);

#ifdef __cpluscplus
}
#endif