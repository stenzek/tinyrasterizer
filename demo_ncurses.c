#include "rasterizer.h"
#include "demo.h"
#include "settings.h"

#if defined(USE_NCURSES)

#include <ncurses.h>
#include <unistd.h>

struct window_data
{
    int width;
    int height;
    struct demo_state *ds;
};

static void demo_ncurses_clear(void *userdata)
{
    struct window_data *wd = (struct window_data *)userdata;

    clear();
}

static void demo_ncurses_set_pixel(void *userdata, int x, int y, unsigned int color)
{
    struct window_data *wd = (struct window_data *)userdata;
    if (x < 0 || x >= wd->width || y < 0 || y >= wd->height)
        return;

#if USE_NCURSES_COLOR
    unsigned int r = (color & 0xFF);
    unsigned int g = (color >> 8) & 0xFF;
    unsigned int b = (color >> 16) & 0xFF;
    int color_pair = 0;
    if (r > g && r > b)
        color_pair = (r > 127) ? COLOR_MAGENTA : COLOR_RED;
    else if (g > b)
        color_pair = (g > 127) ? COLOR_YELLOW : COLOR_GREEN;
    else
        color_pair = (b > 127) ? COLOR_CYAN : COLOR_BLUE;

    attron(COLOR_PAIR(color_pair));
    mvaddch(y, x, '*');
    attroff(COLOR_PAIR(color_pair));
#else
    mvaddch(y, x, '*');
#endif

    //usleep(200);
    //refresh();
}

static void demo_ncurses_present(void *userdata)
{
    struct window_data *wd = (struct window_data *)userdata;
    refresh();
}

int main(int argc, char *argv[])
{
    initscr();
    curs_set(0);
    clear();

#if USE_NCURSES_COLOR
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
#endif

    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    halfdelay(1);

    struct window_data *wd = (struct window_data *)malloc(sizeof(struct window_data));
    wd->width = getmaxx(stdscr);
    wd->height = getmaxy(stdscr);

    struct rasterizer_functions rsf;
    rsf.clear = demo_ncurses_clear;
    rsf.set_pixel = demo_ncurses_set_pixel;
    rsf.present = demo_ncurses_present;
    rsf.userdata = wd;
    wd->ds = demo_init(wd->width, wd->height, &rsf);

    refresh();

    for (;;)
    {
        int ch = getch();
        if (ch == KEY_UP)
            demo_rotate_up(wd->ds);
        else if (ch == KEY_DOWN)
            demo_rotate_down(wd->ds);
        else if (ch == KEY_LEFT)
            demo_rotate_left(wd->ds);
        else if (ch == KEY_RIGHT)
            demo_rotate_right(wd->ds);
        else if (ch == 'q')
            break;

        int width = getmaxx(stdscr);
        int height = getmaxy(stdscr);
        if (width != wd->width || height != wd->height)
        {
            wd->width = width;
            wd->height = height;
            demo_reshape(wd->ds, wd->width, wd->height);
        }

        demo_frame(wd->ds);
        //usleep(SLEEP_TIME * 1000);
    }

    free(wd);
    endwin();
    return 0;
}

#endif


