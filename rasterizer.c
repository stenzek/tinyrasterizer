#include "rasterizer.h"
#include "settings.h"
#include <math.h>
#include <string.h>

// we have our own min/max
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

static unsigned int rasterizer_lerp_color(unsigned int color1, unsigned int color2, float factor)
{
    float c1f[4] = { (float)(color1 & 0xFF) / 255.0f, (float)((color1 >> 8) & 0xFF) / 255.0f, (float)((color1 >> 16) & 0xFF) / 255.0f, (float)((color1 >> 24) & 0xFF) / 255.0f };
    float c2f[4] = { (float)(color2 & 0xFF) / 255.0f, (float)((color2 >> 8) & 0xFF) / 255.0f, (float)((color2 >> 16) & 0xFF) / 255.0f, (float)((color2 >> 24) & 0xFF) / 255.0f };
    float cof[4];
    for (int i = 0; i < 4; i++)
    {
        if ((cof[i] = c1f[i] + c2f[i] - c1f[i] * factor) > 1.0f)
            cof[i] = 1.0f;
    }

    return MAKE_COLOR_R8G8B8A8_UNORM((unsigned int)(cof[0] * 255.0f), (unsigned int)(cof[1] * 255.0f), (unsigned int)(cof[2] * 255.0f), (unsigned int)(cof[3] * 255.0f));
}

static unsigned int rasterizer_interpolate_color(unsigned int color1, unsigned int color2, unsigned int color3, float factor1, float factor2, float factor3)
{
    float c1f[4] = { (float)(color1 & 0xFF) / 255.0f, (float)((color1 >> 8) & 0xFF) / 255.0f, (float)((color1 >> 16) & 0xFF) / 255.0f, (float)((color1 >> 24) & 0xFF) / 255.0f };
    float c2f[4] = { (float)(color2 & 0xFF) / 255.0f, (float)((color2 >> 8) & 0xFF) / 255.0f, (float)((color2 >> 16) & 0xFF) / 255.0f, (float)((color2 >> 24) & 0xFF) / 255.0f };
    float c3f[4] = { (float)(color3 & 0xFF) / 255.0f, (float)((color3 >> 8) & 0xFF) / 255.0f, (float)((color3 >> 16) & 0xFF) / 255.0f, (float)((color3 >> 24) & 0xFF) / 255.0f };

    float cof[4];
    for (int i = 0; i < 4; i++)
    {
        if ((cof[i] = c1f[i] * factor1 + c2f[i] * factor2 + c3f[i] * factor3) > 1.0f)
            cof[i] = 1.0f;
    }

    return MAKE_COLOR_R8G8B8A8_UNORM((unsigned int)(cof[0] * 255.0f), (unsigned int)(cof[1] * 255.0f), (unsigned int)(cof[2] * 255.0f), (unsigned int)(cof[3] * 255.0f));
}

static void rasterizer_xform_vertex(const struct rasterizer_state *rs, const rasterizer_vertex *in_vertex, rasterizer_vertex *out_vertex)
{
    vec4 temp;
    vec4_set(&temp, in_vertex->x, in_vertex->y, in_vertex->z, 1.0f);

    // to projection space
    mat4x4_mul_vec4(&temp, &rs->projection_matrix, mat4x4_mul_vec4(&temp, &rs->view_matrix, mat4x4_mul_vec4(&temp, &rs->world_matrix, &temp)));
    temp.x /= temp.w;
    temp.y /= temp.w;
    temp.z /= temp.w;

    // to viewport space
    float x = (float)rs->viewport.top_left_x + (1.0f + temp.x) * (float)rs->viewport.width / 2.0f;
    float y = (float)rs->viewport.top_left_y + (1.0f - temp.y) * (float)rs->viewport.height / 2.0f;
    float z = 0.0f + temp.z * (1.0f - 0.0f);
    out_vertex->x = x;
    out_vertex->y = y;
    out_vertex->z = z;
    out_vertex->color = in_vertex->color;
}

void rasterizer_draw_screen_line(const struct rasterizer_state *rs, float x1, float y1, unsigned int color1, float x2, float y2, unsigned int color2)
{
    // http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    float xdiff = x2 - x1;
    float ydiff = y2 - y1;
    if (fabsf(xdiff) > fabsf(ydiff))
    {
        float xmin;
        float xmax;
        if (x1 < x2)
        {
            xmin = x1;
            xmax = x2;
        }
        else
        {
            xmin = x2;
            xmax = x1;
        }

        float slope = ydiff / xdiff;
        for (float x = xmin; x <= xmax; x += 1.0f)
        {
            float y = y1 + (x - x1) * slope;
#if defined(COLOR_INTERPOLATION)
            unsigned int color = rasterizer_lerp_color(color1, color2, ((x - x1) / xdiff));
#else
            unsigned int color = MAKE_COLOR_R8G8B8_UNORM(255, 255, 255);
#endif
            rs->functions.set_pixel(rs->functions.userdata, (int)x, (int)y, color);
        }
    }
    else
    {
        float ymin;
        float ymax;
        if (y1 < y2)
        {
            ymin = y1;
            ymax = y2;
        }
        else
        {
            ymin = y2;
            ymax = y1;
        }

        float slope = xdiff / ydiff;
        for (float y = ymin; y <= ymax; y += 1.0f)
        {
            float x = x1 + (y - y1) * slope;
#if defined(COLOR_INTERPOLATION)
            unsigned int color = rasterizer_lerp_color(color1, color2, ((y - y1) / ydiff));
#else
            unsigned int color = MAKE_COLOR_R8G8B8_UNORM(255, 255, 255);
#endif

            rs->functions.set_pixel(rs->functions.userdata, (int)x, (int)y, color);
        }
    }
}

void rasterizer_draw_line(const struct rasterizer_state *rs, const rasterizer_vertex verts[2])
{
    // transform to world, then view, then projection space
    rasterizer_vertex start, end;
    rasterizer_xform_vertex(rs, &verts[0], &start);
    rasterizer_xform_vertex(rs, &verts[1], &end);

    // really basic culling
    if (start.z < 0.0f && end.z < 0.0f)
        return;

    // draw it
    rasterizer_draw_screen_line(rs, start.x, start.y, start.color, end.x, end.y, end.color);
}

void rasterizer_draw_line_list(const struct rasterizer_state *rs, const rasterizer_vertex *verts, size_t nverts)
{
    for (size_t start = 0; start < nverts; start += 2)
        rasterizer_draw_line(rs, verts + start);
}

void rasterizer_draw_triangle(const struct rasterizer_state *rs, const rasterizer_vertex verts[3])
{
    // transform to world, then view, then projection space
    rasterizer_vertex projected_vertices[3];
    for (int i = 0; i < 3; i++)
        rasterizer_xform_vertex(rs, &verts[i], &projected_vertices[i]);

#if 1
    // https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
#define min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define max(v1, v2) ((v1) > (v2) ? (v1) : (v2))
#define min3(v1, v2, v3) (((v1) < (v2)) ? (((v1) < (v3)) ? (v1) : (v3)) : (((v2) < (v3)) ? (v2) : (v3)))
#define max3(v1, v2, v3) (((v1) > (v2)) ? (((v1) > (v3)) ? (v1) : (v3)) : (((v2) > (v3)) ? (v2) : (v3)))
#define orient2d(ax, ay, bx, by, cx, cy) ((bx - ax) * (cy - ay) - (by - ay) * (cx - ax))

    // round to integers
    for (int i = 0; i < 3; i++)
    {
//         projected_vertices[i].x = floorf(projected_vertices[i].x + 0.5f);
//         projected_vertices[i].y = floorf(projected_vertices[i].y + 0.5f);
//         projected_vertices[i].z = floorf(projected_vertices[i].z + 0.5f);
        projected_vertices[i].x = floorf(projected_vertices[i].x);
        projected_vertices[i].y = floorf(projected_vertices[i].y);
        projected_vertices[i].z = floorf(projected_vertices[i].z);
    }

    // calculate triangle bounding box
    int minX = min3((int)projected_vertices[0].x, (int)projected_vertices[1].x, (int)projected_vertices[2].x);
    int minY = min3((int)projected_vertices[0].y, (int)projected_vertices[1].y, (int)projected_vertices[2].y);
    int maxX = max3((int)projected_vertices[0].x, (int)projected_vertices[1].x, (int)projected_vertices[2].x);
    int maxY = max3((int)projected_vertices[0].y, (int)projected_vertices[1].y, (int)projected_vertices[2].y);

    // clip against screen bounds
    minX = max(minX, 0);
    maxX = min(maxX, rs->viewport.width - 1);
    minY = max(minY, 0);
    maxY = min(maxY, rs->viewport.height - 1);

    // rasterize
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            int w0 = orient2d((int)projected_vertices[1].x, (int)projected_vertices[1].y, (int)projected_vertices[2].x, (int)projected_vertices[2].y, x, y);
            int w1 = orient2d((int)projected_vertices[2].x, (int)projected_vertices[2].y, (int)projected_vertices[0].x, (int)projected_vertices[0].y, x, y);
            int w2 = orient2d((int)projected_vertices[0].x, (int)projected_vertices[0].y, (int)projected_vertices[1].x, (int)projected_vertices[1].y, x, y);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
#if defined(COLOR_INTERPOLATION)
                // interpolate color
                int S = w0 + w1 + w2;
                float factor1 = (float)(w1 / (float)S);
                float factor2 = (float)(w2 / (float)S);
                float factor3 = (float)(w0 / (float)S);
                unsigned int color = rasterizer_interpolate_color(projected_vertices[0].color, projected_vertices[1].color, projected_vertices[2].color, factor1, factor2, factor3);
#else
                unsigned int color = MAKE_COLOR_R8G8B8_UNORM(255, 255, 255);
#endif

                rs->functions.set_pixel(rs->functions.userdata, x, y, color);
            }
        }
    }

#undef min
#undef min3
#undef max3
#undef orient2d

#endif
}

void rasterizer_draw_triangle_list(const struct rasterizer_state *rs, const rasterizer_vertex *verts, size_t nverts)
{
    for (size_t start = 0; start < nverts; start += 3)
        rasterizer_draw_triangle(rs, verts + start);
}

