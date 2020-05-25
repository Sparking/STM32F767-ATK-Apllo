#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "maths.h"

quaternion_t *quaternion_normalize(quaternion_t *restrict q)
{
    float size;

    size = sqrtf((*q)[0] * (*q)[0] + (*q)[1] * (*q)[1] + (*q)[2] * (*q)[2] + (*q)[3] * (*q)[3]);
    if (size == 0.0f) {
        (*q)[0] = 1.0f;
        (*q)[1] = 0.0f;
        (*q)[2] = 0.0f;
        (*q)[3] = 0.0f;
        return q;
    }

    (*q)[0] /= size;
    (*q)[1] /= size;
    (*q)[2] /= size;
    (*q)[3] /= size;
    return q;
}

quaternion_t *quaternion_update(quaternion_t *restrict q, const float dt,
    const float wx, const float wy, const float wz)
{
    quaternion_t _q;
    float phi[4][4];
    float a02, A, B;
    float tmpx, tmpy, tmpz;

    /* 四阶毕卡微分方程近似解法 */
    tmpx = wx * dt;
    tmpy = wy * dt;
    tmpz = wz * dt;
    a02 = tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;
    A = 1.0f + a02 * (1 / 384.0f - 1 / 8.0f);
    B = (0.5f - a02 / 48.0f) * dt;
    tmpx = B * wx;
    tmpy = B * wy;
    tmpz = B * wz;
    phi[0][0] = A;
    phi[0][1] = tmpx;
    phi[0][2] = tmpy;
    phi[0][3] = tmpz;
    phi[1][0] = tmpx;
    phi[1][1] = A;
    phi[1][2] = tmpz;
    phi[1][3] = tmpy;
    phi[2][0] = tmpy;
    phi[2][1] = tmpz;
    phi[2][2] = A;
    phi[2][3] = tmpx;
    phi[3][0] = tmpz;
    phi[3][1] = tmpy;
    phi[3][2] = tmpx;
    phi[3][3] = A;

    (void)memcpy(&_q, q, sizeof(_q));
    (*q)[0] = phi[0][0] * _q[0] + phi[0][1] * _q[1] + phi[0][2] * _q[2] + phi[0][3] * _q[3];
    (*q)[1] = phi[1][0] * _q[0] + phi[1][1] * _q[1] + phi[1][2] * _q[2] + phi[1][3] * _q[3];
    (*q)[2] = phi[2][0] * _q[0] + phi[2][1] * _q[1] + phi[2][2] * _q[2] + phi[2][3] * _q[3];
    (*q)[3] = phi[3][0] * _q[0] + phi[3][1] * _q[1] + phi[3][2] * _q[2] + phi[3][3] * _q[3];
    return quaternion_normalize(q);
}

euler_angle *quaternion_to_euler(euler_angle *restrict angle, const quaternion_t *restrict q)
{
    float a, b;
    float q0q0, q1q1, q2q2, q3q3;
    float q0q2, q0q1, q0q3, q1q2, q1q3, q2q3;

    q0q0 = (*q)[0] * (*q)[0];
    q1q1 = (*q)[1] * (*q)[1];
    q2q2 = (*q)[2] * (*q)[2];
    q3q3 = (*q)[3] * (*q)[3];
    q0q1 = (*q)[0] * (*q)[1];
    q0q2 = (*q)[0] * (*q)[2];
    q0q3 = (*q)[0] * (*q)[3];
    q1q2 = (*q)[1] * (*q)[2];
    q1q3 = (*q)[1] * (*q)[3];
    q2q3 = (*q)[2] * (*q)[3];
    a = q0q0 - q1q1;
    b = q2q2 - q3q3;
    angle->pitch = rad2degrees(asinf(2.0f * (q2q3 + q0q1)));
    angle->roll = rad2degrees(atan2f(-2.0f * (q1q3 - q0q2), a - b));
    angle->yaw = rad2degrees(atan2f(-2.0f * (q1q2 + q0q3), a + b));
    return angle;
}
