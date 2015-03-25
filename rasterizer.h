#pragma once
#include "minimath.h"
#include <stdlib.h>

typedef void(*rs_clear_fn)(void *userdata);
typedef void(*rs_set_pixel_fn)(void *userdata, int x, int y, unsigned int color);
typedef void(*rs_present_fn)(void *userdata);

struct viewport_state
{
    int top_left_x;
    int top_left_y;
    int width;
    int height;
};

struct rasterizer_functions
{
    rs_clear_fn clear;
    rs_set_pixel_fn set_pixel;
    rs_present_fn present;
    void *userdata;
};

struct rasterizer_state
{
    mat4x4 world_matrix;
    mat4x4 view_matrix;
    mat4x4 projection_matrix;
    struct viewport_state viewport;

    struct rasterizer_functions functions;
};

typedef struct
{
    float x, y, z;
    unsigned int color;
} rasterizer_vertex;

#define MAKE_COLOR_R8G8B8_UNORM(r, g, b) ((unsigned int)0xFF000000 | ((unsigned int)(b) << 16) | ((unsigned int)(g) << 8) | ((unsigned int)(r)) )
#define MAKE_COLOR_R8G8B8A8_UNORM(r, g, b, a) ( ((unsigned int)(a) << 24) | ((unsigned int)(b) << 16) | ((unsigned int)(g) << 8) | ((unsigned int)(r)) )

void rasterizer_draw_screen_line(const struct rasterizer_state *rs, float x1, float y1, unsigned int color1, float x2, float y2, unsigned int color2);

void rasterizer_draw_line(const struct rasterizer_state *rs, const rasterizer_vertex verts[2]);
void rasterizer_draw_line_list(const struct rasterizer_state *rs, const rasterizer_vertex *verts, size_t nverts);

void rasterizer_draw_triangle(const struct rasterizer_state *rs, const rasterizer_vertex verts[3]);
void rasterizer_draw_triangle_list(const struct rasterizer_state *rs, const rasterizer_vertex *verts, size_t nverts);

