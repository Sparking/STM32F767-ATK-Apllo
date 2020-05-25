#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float pitch;    /* 俯仰角 */
    float roll;     /* 横滚角 */
    float yaw;      /* 偏航角 */
} euler_angle;

typedef float quaternion_t[4];

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
 * @dt 采样间隔时间
 * @wx x轴角速度
 * @wy y轴角速度
 * @wz z轴角速度
 * 返回四元数的地址
 */
extern quaternion_t *quaternion_update(quaternion_t *restrict q, const float dt,
    const float wx, const float wy, const float wz);

/**
 * quaternion_to_euler 四元数求欧拉角
 *
 * @angle 欧拉角，弧度值
 * @q 四元数
 * 返回欧拉角的地址
 */
extern euler_angle *quaternion_to_euler(euler_angle *restrict angle, const quaternion_t *restrict q);

#ifdef __cplusplus
}
#endif

#endif /* __QUATERNION_H__ */
