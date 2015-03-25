#pragma once
#include "rasterizer.h"

struct demo_state *demo_init(int screenw, int screenh, struct rasterizer_functions *functions);
void demo_reshape(struct demo_state *ds, int screenw, int screenh);
void demo_rotate_up(struct demo_state *ds);
void demo_rotate_down(struct demo_state *ds);
void demo_rotate_left(struct demo_state *ds);
void demo_rotate_right(struct demo_state *ds);
void demo_frame(struct demo_state *ds);

