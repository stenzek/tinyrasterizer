#include "minimath.h"
#include <math.h>
#include <string.h>
#define Y_PI (3.14159265358979323846f)

vec4 *vec4_zero(vec4 *dst)
{
    dst->x = dst->y = dst->z = dst->w = 0.0f;
    return dst;
}

vec4 *vec4_copy(vec4 *dst, const vec4 *src)
{
    memcpy(dst->components, src->components, sizeof(dst->components));
    return dst;
}

vec4 *vec4_set(vec4 *dst, float x, float y, float z, float w)
{
    dst->x = x;
    dst->y = y;
    dst->z = z;
    dst->w = w;
    return dst;
}

vec4 *vec4_add(vec4 *dst, const vec4 *lhs, const vec4 *rhs)
{
    dst->x = lhs->x + rhs->x;
    dst->y = lhs->y + rhs->y;
    dst->z = lhs->z + rhs->z;
    dst->w = lhs->w + rhs->w;
    return dst;
}

vec4 *vec4_sub(vec4 *dst, const vec4 *lhs, const vec4 *rhs)
{
    dst->x = lhs->x - rhs->x;
    dst->y = lhs->y - rhs->y;
    dst->z = lhs->z - rhs->z;
    dst->w = lhs->w - rhs->w;
    return dst;
}

vec4 *vec4_mul(vec4 *dst, const vec4 *lhs, const vec4 *rhs)
{
    dst->x = lhs->x * rhs->x;
    dst->y = lhs->y * rhs->y;
    dst->z = lhs->z * rhs->z;
    dst->w = lhs->w * rhs->w;
    return dst;
}

vec4 *vec4_div(vec4 *dst, const vec4 *lhs, const vec4 *rhs)
{
    dst->x = lhs->x / rhs->x;
    dst->y = lhs->y / rhs->y;
    dst->z = lhs->z / rhs->z;
    dst->w = lhs->w / rhs->w;
    return dst;
}

float vec4_dot(const vec4 *lhs, const vec4 *rhs)
{
    return (lhs->x * rhs->x) + (lhs->y * rhs->y) + (lhs->z * rhs->z) + (lhs->w * rhs->w);
}

float vec4_length(const vec4 *vec)
{
    float sq_length = vec4_dot(vec, vec);
    return sqrtf(sq_length);
}

mat4x4 *mat4x4_identity(mat4x4 *dst)
{
    vec4_set(&dst->rows[0], 1.0f, 0.0f, 0.0f, 0.0f);
    vec4_set(&dst->rows[1], 0.0f, 1.0f, 0.0f, 0.0f);
    vec4_set(&dst->rows[2], 0.0f, 0.0f, 1.0f, 0.0f);
    vec4_set(&dst->rows[3], 0.0f, 0.0f, 0.0f, 1.0f);
    return dst;
}

vec4 *mat4x4_row(vec4 *dst, const mat4x4 *mat, size_t row)
{
    return vec4_copy(dst, &mat->rows[row]);
}

vec4 *mat4x4_col(vec4 *dst, const mat4x4 *mat, size_t col)
{
    return vec4_set(dst, mat->data[0][col], mat->data[1][col], mat->data[2][col], mat->data[3][col]);
}

mat4x4 *mat4x4_mul(mat4x4 *dst, const mat4x4 *lhs, const mat4x4 *rhs)
{
    vec4 tmp1, tmp2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dst->data[i][j] = vec4_dot(mat4x4_row(&tmp1, lhs, i), mat4x4_col(&tmp2, rhs, j));
        }
    }

    return dst;
}

vec4 *mat4x4_mul_vec4(vec4 *dst, const mat4x4 *lhs, const vec4 *rhs)
{
    float x = vec4_dot(&lhs->rows[0], rhs);
    float y = vec4_dot(&lhs->rows[1], rhs);
    float z = vec4_dot(&lhs->rows[2], rhs);
    float w = vec4_dot(&lhs->rows[3], rhs);
    vec4_set(dst, x, y, z, w);
    return dst;
}

mat4x4 *mat4x4_rotate_x(mat4x4 *dst, float angle)
{
    float rads = angle * (Y_PI / 180.0f);
    float s = sinf(rads);
    float c = cosf(rads);

    vec4_set(&dst->rows[0], 1.0f, 0.0f, 0.0f, 0.0f);
    vec4_set(&dst->rows[1], 0.0f, c, -s, 0.0f);
    vec4_set(&dst->rows[2], 0.0f, s, c, 0.0f);
    vec4_set(&dst->rows[3], 0.0f, 0.0f, 0.0f, 1.0f);
    return dst;
}

mat4x4 *mat4x4_rotate_y(mat4x4 *dst, float angle)
{
    float rads = angle * (Y_PI / 180.0f);
    float s = sinf(rads);
    float c = cosf(rads);

    vec4_set(&dst->rows[0], c, 0.0f, s, 0.0f);
    vec4_set(&dst->rows[1], 0.0f, 1.0f, 0.0f, 0.0f);
    vec4_set(&dst->rows[2], -s, 0.0f, c, 0.0f);
    vec4_set(&dst->rows[3], 0.0f, 0.0f, 0.0f, 1.0f);
    return dst;
}

mat4x4 *mat4x4_translate(mat4x4 *dst, float x, float y, float z)
{
    vec4_set(&dst->rows[0], 1.0f, 0.0f, 0.0f, x);
    vec4_set(&dst->rows[1], 0.0f, 1.0f, 0.0f, y);
    vec4_set(&dst->rows[2], 0.0f, 0.0f, 1.0f, z);
    vec4_set(&dst->rows[3], 0.0f, 0.0f, 0.0f, 1.0f);
    return dst;
}

mat4x4 *mat4x4_ortho(mat4x4 *dst, float width, float height, float znear, float zfar)
{
    float hw = width / 2.0f;
    float hh = height / 2.0f;
    float left = -hw;
    float right = hw;
    float bottom = -hh;
    float top = hh;

    vec4_set(&dst->rows[0], 2.0f / (right - left), 0.0f, 0.0f, (left + right) / (left - right));
    vec4_set(&dst->rows[1], 0.0f, 2.0f / (top - bottom), 0.0f, (top + bottom) / (bottom - top));
    vec4_set(&dst->rows[2], 0.0f, 0.0f, 1.0f / (znear - zfar), znear / (znear - zfar));
    vec4_set(&dst->rows[3], 0.0f, 0.0f, 0.0f, 1.0f);
    return dst;
}

mat4x4 *mat4x4_perspective(mat4x4 *dst, float fov, float aspect, float znear, float zfar)
{
    float fov_rads = fov * (Y_PI / 180.0f);
    float half_fov = fov_rads / 2.0f;
    float sin_half_fov = sinf(fov_rads);
    float cos_half_fov = cosf(fov_rads);

    float y_scale = cos_half_fov / sin_half_fov;
    float x_scale = y_scale / aspect;

    vec4_set(&dst->rows[0], x_scale, 0.0f, 0.0f, 0.0f);
    vec4_set(&dst->rows[1], 0.0f, y_scale, 0.0f, 0.0f);
    vec4_set(&dst->rows[2], 0.0f, 0.0f, zfar / (znear - zfar), znear * zfar / (znear - zfar));
    vec4_set(&dst->rows[3], 0.0f, 0.0f, -1.0f, 0.0f);
    return dst;
}

