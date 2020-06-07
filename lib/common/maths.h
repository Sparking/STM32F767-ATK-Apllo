#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>

#define MATHS_SUPPORT_DOUBLE_PRECISION

#if defined( MATHS_SUPPORT_DOUBLE_PRECISION )
#define FLOAT   double
#define M_PIf   acos(-1.0L)
#define SQRTF   sqrt
#define FABSF   fabs
#define ASINF   asin
#define ATAN2F  atan2
#else
#define FLOAT   float
#define M_PIf   acosf(-1.0f)
#define SQRTF   sqrtf
#define FABSF   fabsf
#define ASINF   asinf
#define ATAN2F  atan2f
#endif

#define QUATERNION_DEFAULT_INITIALIZER      {1.0, 0.0, 0.0, 0.0}
#define QUATERNION_DEFAULT(q)               \
    quaternion_t q = QUATERNION_DEFAULT_INITIALIZER

struct point {
    int x;
    int y;
};

typedef struct {
    short x;
    short y;
    short z;
} hvector3d_t;

typedef struct {
    FLOAT x;
    FLOAT y;
    FLOAT z;
} fvector3d_t;

typedef struct {
    FLOAT pitch;    /* 俯仰角 */
    FLOAT roll;     /* 横滚角 */
    FLOAT yaw;      /* 偏航角 */
} euler_angle_t;

typedef FLOAT quaternion_t[4];

static inline FLOAT degrees2radians(const FLOAT degrees)
{
    return degrees * (M_PIf / 180.0f);
}

static inline FLOAT radians2degrees(const FLOAT rad)
{
    return rad * (180.0f / M_PIf);
}

extern FLOAT fvector3d_size(const fvector3d_t *v);

extern fvector3d_t *fvector3d_add(fvector3d_t *res, const fvector3d_t *v, const fvector3d_t *restrict value);

extern fvector3d_t *fvector3d_scale(fvector3d_t *res, const fvector3d_t *v, const FLOAT t);

extern fvector3d_t *fvector3d_normalize(fvector3d_t *res, const fvector3d_t *v);

extern hvector3d_t *hvector3d_add(hvector3d_t *res, const hvector3d_t *v, const hvector3d_t *restrict value);

extern hvector3d_t *hvector3d_scale(hvector3d_t *res, const hvector3d_t *v, const short t);

extern hvector3d_t *hvector3d_scale3x(hvector3d_t *res, const hvector3d_t *v, const hvector3d_t *restrict t);

/**
 * quaternion_normalize 四元数归一化
 *
 * @q 四元数
 * 返回四元数的地址
 */
extern quaternion_t *quaternion_normalize(quaternion_t *restrict q);

/**
 * quaternion_update 四元数更新，使用四阶毕卡解法
 * 
 * @q 四元数
 * @delta 3轴角增量
 * 返回四元数的地址
 */
extern quaternion_t *quaternion_update(quaternion_t *restrict q, const fvector3d_t *restrict delta);

/**
 * quaternion_update 四元数更新，使用四阶毕卡解法
 * 
 * @q 四元数
 * @w 3轴角速度
 * @dt 采样间隔时间
 * 返回四元数的地址
 */
extern quaternion_t *quaternion_update_raw(quaternion_t *restrict q, const fvector3d_t *restrict w,
    const FLOAT dt);

/**
 * quaternion_to_euler 四元数求欧拉角
 *
 * @angle 欧拉角，弧度值
 * @q 四元数
 * 返回欧拉角的地址
 */
extern euler_angle_t *quaternion_to_euler(euler_angle_t *restrict angle, const quaternion_t *restrict q);

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
extern int gaussian_elimination(FLOAT *restrict matrix, FLOAT *restrict res, const unsigned int rows);

#ifdef __cplusplus
}
#endif
