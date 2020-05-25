#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include "quaternion.h"

/* 定义Π的值 */
#define M_PIf               acosf(-1.0f)

struct point {
    int x;
    int y;
};

struct fvector3d {
    float x;
    float y;
    float z;
};
typedef struct fvector3d fvector3d_t;

static inline float degrees2rad(const float degrees)
{
    return degrees * (M_PIf / 180.0f);
}

static inline float rad2degrees(const float rad)
{
    return rad * (180.0f / M_PIf);
}

/**
 * @brief bits_count 计算出一个整数中1的位数
 */
extern unsigned char bits_count(unsigned int i);

/**
 * @brief gcd 求解最大公约数
 */
extern int gcd(const int a, const int b);

/**
 * @brief gaussian_elimination 高斯消元法求解
 * @param matrix 矩阵, 增广矩阵[a * x | b], 其中 [a * x]是个rows*rows行的方阵, [b]是个长度位rows的列向量
 * @param res 解的存放位置, 是个长度为rows的数组
 * @param rows 增广矩阵的行数
 * @return 方程有解返回，如果没有解，返回-1
 */
extern int gaussian_elimination(float *matrix, float *res, const unsigned int rows);

#ifdef __cplusplus
}
#endif
