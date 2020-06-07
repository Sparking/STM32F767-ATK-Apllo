#include <math.h>
#include "maths.h"

quaternion_t *quaternion_normalize(quaternion_t *restrict q)
{
    double size;

    size = SQRTF((*q)[0] * (*q)[0] + (*q)[1] * (*q)[1] + (*q)[2] * (*q)[2] + (*q)[3] * (*q)[3]);
    (*q)[0] /= size;
    (*q)[1] /= size;
    (*q)[2] /= size;
    (*q)[3] /= size;

    return q;
}

quaternion_t *quaternion_update(quaternion_t *restrict q, const fvector3d_t *restrict delta)
{
    FLOAT a, b;
    FLOAT delta2;
    quaternion_t _q;
    fvector3d_t tmp;

    _q[0] = (*q)[0];
    _q[1] = (*q)[1];
    _q[2] = (*q)[2];
    _q[3] = (*q)[3];
    delta2 = fvector3d_size(delta);
    a = 1.0f - delta2 * (1.0f / 8.0f - delta2 / 384.0f);
    b = 0.5f - delta2 / 48.0f;
    fvector3d_scale(&tmp, delta, b);
    (*q)[0] = a * _q[0] - tmp.x * _q[1] - tmp.y * _q[2] - tmp.z * _q[3];
    (*q)[1] = tmp.x * _q[0] + a * _q[1] + tmp.z * _q[2] - tmp.y * _q[3];
    (*q)[2] = tmp.y * _q[0] - tmp.z * _q[1] + a * _q[2] + tmp.x * _q[3];
    (*q)[3] = tmp.z * _q[0] + tmp.y * _q[1] - tmp.x * _q[2] + a * _q[3];

    return quaternion_normalize(q);
}

quaternion_t *quaternion_update_raw(quaternion_t *restrict q, const fvector3d_t *restrict w, const FLOAT dt)
{
    fvector3d_t delta;

    fvector3d_scale(&delta, w, dt);

    return quaternion_update(q, &delta);
}

euler_angle_t *quaternion_to_euler(euler_angle_t *restrict angle, const quaternion_t *restrict q)
{
    FLOAT a, b;
    FLOAT q0q0, q1q1, q2q2, q3q3;
    FLOAT q0q2, q0q1, q0q3, q1q2, q1q3, q2q3;

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
    angle->pitch = radians2degrees(ASINF(2.0f * (q2q3 + q0q1)));
    angle->roll = radians2degrees(ATAN2F(-2.0f * (q1q3 - q0q2), a - b));
    angle->yaw = radians2degrees(ATAN2F(-2.0f * (q1q2 + q0q3), a + b));

    return angle;
}
