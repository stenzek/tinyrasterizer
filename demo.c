#include "demo.h"
#include <string.h>
#include <math.h>

struct demo_state
{
    int screenw;
    int screenh;
    struct rasterizer_state rs;
    float rotation_x;
    float rotation_y;
    int frame_counter;
};

void demo_reshape(struct demo_state *ds, int screenw, int screenh);
void demo_set_world_matrix(struct demo_state *ds, int index);
void demo_set_view_matrix(struct demo_state *ds);
void draw_wire_box(struct demo_state *ds);
void demo_frame(struct demo_state *ds);

struct demo_state *demo_init(int screenw, int screenh, struct rasterizer_functions *functions)
{
    struct demo_state *ds = (struct demo_state *)malloc(sizeof(struct demo_state));
    memcpy(&ds->rs.functions, functions, sizeof(ds->rs.functions));
    mat4x4_identity(&ds->rs.world_matrix);
    mat4x4_identity(&ds->rs.view_matrix);
    ds->rotation_x = 45.0f;
    ds->rotation_y = 0.0f;
    ds->frame_counter = 0;
    demo_reshape(ds, screenw, screenh);
    return ds;
}

void demo_reshape(struct demo_state *ds, int screenw, int screenh)
{
    ds->screenw = screenw;
    ds->screenh = screenh;

    ds->rs.viewport.top_left_x = 0;
    ds->rs.viewport.top_left_y = 0;
    ds->rs.viewport.width = screenw;
    ds->rs.viewport.height = screenh;
    
    mat4x4_ortho(&ds->rs.projection_matrix, 6.0f, 6.0f, 0.1f, 10.0f);
    //mat4x4_perspective(&ds->rs.projection_matrix, 90.0f, (float)screenw / (float)screenh, 0.1f, 10.0f);
}

void demo_rotate_up(struct demo_state *ds)
{
    ds->rotation_x = fmodf(ds->rotation_x + 5.0f, 360.0f);
}

void demo_rotate_down(struct demo_state *ds)
{
    ds->rotation_x = fmodf(ds->rotation_x - 5.0f, 360.0f);
}

void demo_rotate_left(struct demo_state *ds)
{
    ds->rotation_y = fmodf(ds->rotation_y - 5.0f, 360.0f);
}

void demo_rotate_right(struct demo_state *ds)
{
    ds->rotation_y = fmodf(ds->rotation_y + 5.0f, 360.0f);
}

void demo_set_world_matrix(struct demo_state *ds, int index)
{
    const int FULL_ROTATION_FRAMES = 150;

    float rotation = ((float)(ds->frame_counter % FULL_ROTATION_FRAMES) / (float)FULL_ROTATION_FRAMES) * 360.0f;
    
    mat4x4 rotation_matrix;
    mat4x4 translation_matrix;
    mat4x4_rotate_y(&rotation_matrix, rotation);
    mat4x4_translate(&translation_matrix, -2.25f + (float)index + (float)index * 0.5f, 0.0f, 0.0f);
    mat4x4_mul(&ds->rs.world_matrix, &translation_matrix, &rotation_matrix);
}

void demo_set_view_matrix(struct demo_state *ds)
{
    mat4x4 rotation_x;
    mat4x4 rotation_y;
    mat4x4_rotate_x(&rotation_x, ds->rotation_x);
    mat4x4_rotate_y(&rotation_y, ds->rotation_y);

    mat4x4 rotation, translation;
    mat4x4_mul(&rotation, &rotation_y, &rotation_x);
    mat4x4_translate(&translation, 0.0f, -1.0f, -1.0f);

    mat4x4_mul(&ds->rs.view_matrix, &rotation, &translation);
}

void draw_wire_box(struct demo_state *ds)
{
    static const rasterizer_vertex cube_verts[] =
    {
        { -0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(0, 0, 255) },
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(0, 0, 255) },
        { -0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(0, 255, 255) },
        { -0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(0, 255, 255) },
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 255) },
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 255) },
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { 0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { 0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { 0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },
    };

    rasterizer_draw_line_list(&ds->rs, cube_verts, sizeof(cube_verts) / sizeof(cube_verts[0]));
}

void draw_box(struct demo_state *ds)
{
    // these are stolen from my game engine which is z-up.. seems to work okay though
    static const rasterizer_vertex cube_verts[] =
    {
        // front face
        { -0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },    // bottom-front-left
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },    // bottom-front-right
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },    // top-front-left
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 0) },    // top-front-left
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },    // bottom-front-right
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 0, 0) },    // top-front-right

        // back face
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-left
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-left
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-right
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-right
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-left
        { 0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-right

        // left face
        { -0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-left
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-left
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-left
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-left
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-left
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-front-left

        // right face
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-right
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-right
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-right
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-right
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-right
        { 0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-front-right

        // top face
        { -0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-front-left
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-front-right
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-left
        { -0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-left
        { 0.5f, -0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-front-right
        { 0.5f, 0.5f, 0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // top-back-right

        // bottom face
        { -0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-left
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-left
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-right
        { 0.5f, -0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-front-right
        { -0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-left
        { 0.5f, 0.5f, -0.5f, MAKE_COLOR_R8G8B8_UNORM(255, 255, 255) },    // bottom-back-right
    };

    rasterizer_draw_triangle_list(&ds->rs, cube_verts, sizeof(cube_verts) / sizeof(cube_verts[0]));
}

void demo_frame(struct demo_state *ds)
{
    ds->frame_counter++;
    demo_set_view_matrix(ds);

    ds->rs.functions.clear(ds->rs.functions.userdata);

    demo_set_world_matrix(ds, 0);
    //draw_wire_box(ds);
    draw_box(ds);

    demo_set_world_matrix(ds, 1);
    draw_wire_box(ds);
    //draw_box(ds);

    demo_set_world_matrix(ds, 2);
    draw_wire_box(ds);
    //draw_box(ds);

    demo_set_world_matrix(ds, 3);
    //draw_wire_box(ds);
    draw_box(ds);

    //rasterizer_draw_line(&ds->rs, 1, 1, 10, 10);
    //
    ds->rs.functions.present(ds->rs.functions.userdata);
}

