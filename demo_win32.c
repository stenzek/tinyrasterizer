#include "rasterizer.h"
#include "demo.h"
#include "settings.h"

#if defined(USE_WIN32)

#include <Windows.h>

#if defined(USE_WIN32) && defined(WIN32_USE_WINDOW)

struct window_data
{
    HWND hwnd;
    HDC hdc;
    HDC paint_dc;
    HBITMAP hbitmap;
    int win_width;
    int win_height;
    struct demo_state *ds;
};

static void demo_win32_clear(void *userdata)
{
    struct window_data *wd = (struct window_data *)userdata;

    RECT rect;
    GetClientRect(wd->hwnd, &rect);
    FillRect(wd->hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

static void demo_win32_set_pixel(void *userdata, int x, int y, unsigned int color)
{
    struct window_data *wd = (struct window_data *)userdata;
    SetPixel(wd->hdc, x, y, color & 0x00FFFFFF);
}

static void demo_win32_present(void *userdata)
{
    struct window_data *wd = (struct window_data *)userdata;

#if defined(WIN32_WINDOW_DOUBLE_BUFFERING)
    BitBlt(wd->paint_dc, 0, 0, wd->win_width, wd->win_height, wd->hdc, 0, 0, SRCCOPY);
#endif
}

static void init_window(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    struct window_data *wd = (struct window_data *)malloc(sizeof(struct window_data));
    wd->hwnd = hwnd;
    wd->hdc = NULL;
    wd->paint_dc = NULL;
    wd->hbitmap = NULL;
    wd->win_width = rect.right - rect.left;
    wd->win_height = rect.bottom - rect.top;

#if defined(WIN32_WINDOW_DOUBLE_BUFFERING)
    HDC hwnd_dc = GetDC(hwnd);
    wd->hdc = CreateCompatibleDC(hwnd_dc);
    wd->hbitmap = CreateCompatibleBitmap(hwnd_dc, wd->win_width, wd->win_height);
    SelectObject(wd->hdc, wd->hbitmap);
    ReleaseDC(hwnd, hwnd_dc);
#endif

    struct rasterizer_functions rsf;
    rsf.clear = demo_win32_clear;
    rsf.set_pixel = demo_win32_set_pixel;
    rsf.present = demo_win32_present;
    rsf.userdata = wd;
    wd->ds = demo_init(wd->win_width, wd->win_height, &rsf);
    
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wd);
    SetTimer(hwnd, 1, SLEEP_TIME, NULL);
}

static void draw_frame(HWND hwnd)
{
    struct window_data *wd = (struct window_data *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    PAINTSTRUCT ps;
    if (!BeginPaint(hwnd, &ps))
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    // handle resize
    int win_width = rect.right - rect.left;
    int win_height = rect.bottom - rect.top;
    if (win_width != wd->win_width || win_height != wd->win_height)
    {
        demo_reshape(wd->ds, win_width, win_height);
        wd->win_width = win_width;
        wd->win_height = win_height;

#if defined(WIN32_WINDOW_DOUBLE_BUFFERING)
        SelectObject(wd->hdc, NULL);
        DeleteObject(wd->hbitmap);
        DeleteDC(wd->hdc);

        wd->hdc = CreateCompatibleDC(ps.hdc);
        wd->hbitmap = CreateCompatibleBitmap(ps.hdc, wd->win_width, wd->win_height);
        SelectObject(wd->hdc, wd->hbitmap);
#endif
    }

#if defined(WIN32_WINDOW_DOUBLE_BUFFERING)
    wd->paint_dc = ps.hdc;
    demo_frame(wd->ds);
    wd->paint_dc = NULL;
#else   
    wd->hdc = ps.hdc;
    demo_frame(wd->ds);
    wd->hdc = NULL;
#endif

    EndPaint(hwnd, &ps);
}

static void cleanup_window(HWND hwnd)
{
    struct window_data *wd = (struct window_data *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

#if defined(WIN32_WINDOW_DOUBLE_BUFFERING)
    SelectObject(wd->hdc, NULL);
    DeleteObject(wd->hbitmap);
    DeleteDC(wd->hdc);
#endif

    free(wd);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        cleanup_window(hwnd);
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CREATE:
        init_window(hwnd);
        return 0;

    case WM_PAINT:
        draw_frame(hwnd);
        return 0;

    case WM_TIMER:
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "demo_win32";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Failed to register window class", "Error", MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "demo_win32", "Rasterizer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Failed to create window", "Error", MB_ICONERROR);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

int main(int argc, char *argv[])
{
    return WinMain(NULL, NULL, "", SW_SHOW);
}

#elif defined(USE_WIN32)

struct window_data
{
    HANDLE handle;
    int width;
    int height;
    struct demo_state *ds;
};

static void demo_win32_clear(void *userdata)
{
    struct window_data *wd = (struct window_data *)userdata;
   
    COORD loc = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacterA(wd->handle, ' ', wd->width * wd->height, loc, &written);
}

static void demo_win32_set_pixel(void *userdata, int x, int y, unsigned int color)
{
    struct window_data *wd = (struct window_data *)userdata;
    if (x < 0 || x >= wd->width || y < 0 || y >= wd->height)
        return;

    COORD loc = { (short)x, (short)y };
    SetConsoleCursorPosition(wd->handle, loc);

    char ch = 'x';
    DWORD written;
    WriteConsoleA(wd->handle, &ch, 1, &written, 0);
}

static void demo_win32_present(void *userdata)
{
    struct window_data *wd = (struct window_data *)userdata;
}

int main(int argc, char *argv[])
{
    //CONSOLE_SCREEN_BUFFER_INFO console_info;
    //GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
    //console_info.dwSize.Y = 80;
    CONSOLE_SCREEN_BUFFER_INFOEX console_infoex;
    console_infoex.cbSize = sizeof(console_infoex);
    GetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE), &console_infoex);

    struct window_data *wd = (struct window_data *)malloc(sizeof(struct window_data));
    wd->handle = GetStdHandle(STD_OUTPUT_HANDLE);
    //wd->width = console_info.dwSize.X;
    //wd->height = console_info.dwSize.Y;
    wd->width = console_infoex.srWindow.Right - console_infoex.srWindow.Left + 1;
    wd->height = console_infoex.srWindow.Bottom - console_infoex.srWindow.Top + 1;

    struct rasterizer_functions rsf;
    rsf.clear = demo_win32_clear;
    rsf.set_pixel = demo_win32_set_pixel;
    rsf.present = demo_win32_present;
    rsf.userdata = wd;
    wd->ds = demo_init(wd->width, wd->height, &rsf);

    for (;;)
    {
        // check for resize
        //GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
        //console_info.dwSize.Y = 80;
        //if (console_info.dwSize.X != wd->width || console_info.dwSize.Y != wd->height)
        GetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE), &console_infoex);
        console_infoex.cbSize = sizeof(console_infoex);
        if ((console_infoex.srWindow.Right - console_infoex.srWindow.Left + 1) != wd->width || (console_infoex.srWindow.Bottom - console_infoex.srWindow.Top + 1) != wd->height)
        {
            //wd->width = console_info.dwSize.X;
            //wd->height = console_info.dwSize.Y;
            wd->width = console_infoex.srWindow.Right - console_infoex.srWindow.Left + 1;
            wd->height = console_infoex.srWindow.Bottom - console_infoex.srWindow.Top + 1;
            demo_reshape(wd->ds, wd->width, wd->height);
        }

        demo_frame(wd->ds);
        Sleep(SLEEP_TIME);
    }

    free(wd);
    return 0;
}

#endif

#endif
