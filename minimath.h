#pragma once
#include <stdlib.h>

typedef union
{
    struct
    {
        float x, y, z, w;
    };
    float components[4];
} vec4;

typedef union
{
    struct
    {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    vec4 rows[4];
    float data[4][4];
} mat4x4;

vec4 *vec4_zero(vec4 *dst);
vec4 *vec4_copy(vec4 *dst, const vec4 *src);
vec4 *vec4_set(vec4 *dst, float x, float y, float z, float w);
vec4 *vec4_add(vec4 *dst, const vec4 *lhs, const vec4 *rhs);
vec4 *vec4_sub(vec4 *dst, const vec4 *lhs, const vec4 *rhs);
vec4 *vec4_mul(vec4 *dst, const vec4 *lhs, const vec4 *rhs);
vec4 *vec4_div(vec4 *dst, const vec4 *lhs, const vec4 *rhs);
float vec4_dot(const vec4 *lhs, const vec4 *rhs);
float vec4_length(const vec4 *vec);

mat4x4 *mat4x4_identity(mat4x4 *dst);
mat4x4 *mat4x4_mul(mat4x4 *dst, const mat4x4 *lhs, const mat4x4 *rhs);
vec4 *mat4x4_row(vec4 *dst, const mat4x4 *mat, size_t row);
vec4 *mat4x4_col(vec4 *dst, const mat4x4 *mat, size_t col);
vec4 *mat4x4_mul_vec4(vec4 *dst, const mat4x4 *lhs, const vec4 *rhs);

mat4x4 *mat4x4_rotate_x(mat4x4 *dst, float angle);
mat4x4 *mat4x4_rotate_y(mat4x4 *dst, float angle);
mat4x4 *mat4x4_translate(mat4x4 *dst, float x, float y, float z); 
mat4x4 *mat4x4_ortho(mat4x4 *dst, float width, float height, float znear, float zfar);
mat4x4 *mat4x4_perspective(mat4x4 *dst, float fov, float aspect, float znear, float zfar);
