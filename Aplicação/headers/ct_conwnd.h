#ifndef CT_CONWND_H
#define CT_CONWND_H

// CONSOLE WINDOW HANDLING HEADER.

// TAGS:
/*
    w : Window;
    m : Monitor;
    c : Cursor;
    i : Input;

    con : Console;

    vkey : Virtual Key;
    key : Key regarding ASCII table;

    hwnd : Window Handler;
*/

#include "ct_definitions.h"
#include "ct_page.h"


HWND ct_get_conw()
{
    return GetConsoleWindow();
}

HANDLE ct_get_wbuffer()
{
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

F_STATUS ct_get_mon_width()
{
    int mon_width = GetSystemMetrics(SM_CXSCREEN);

    return (mon_width == 0) ? F_ERROR : mon_width;
}

F_STATUS ct_get_mon_height()
{
    int mon_height = GetSystemMetrics(SM_CYSCREEN);

    return (mon_height == 0) ? F_ERROR : mon_height;
}

F_STATUS ct_set_wstyle_fixed(HWND hwnd)
{
    LONG wstyle = GetWindowLong(hwnd, GWL_STYLE);
    F_STATUS f_status;

    if (wstyle == 0)
        return F_ERROR;

    wstyle = wstyle & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, wstyle);

    return F_SUCCESS;
}

void ct_setw_maximized(HWND hwnd)
{
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

F_STATUS ct_format_win(HWND hwnd, LPCTSTR wname, int width, int height, BOOL fixedw)
{
    F_STATUS f_status;
    int x_centre, y_centre;

    x_centre = ct_get_mon_width();
    y_centre = ct_get_mon_height();

    if (x_centre == 0 || y_centre == 0)
        return F_ERROR;

    x_centre = (x_centre / 2) - (width / 2);
    y_centre = (y_centre / 2) - (height / 2);

    f_status = MoveWindow(hwnd, x_centre, y_centre, width, height, TRUE);

    if (f_status == 0)
        return F_ERROR;

    if (wname == NULL)
        f_status = SetConsoleTitle("DEFAULT WINDOW");
    else
        f_status = SetConsoleTitle(wname);

    if (f_status == 0)
        return F_ERROR;

    if (fixedw)
    {
        f_status = ct_set_wstyle_fixed(hwnd);

        if (f_status != F_SUCCESS)
            return F_ERROR;
    }

    return F_SUCCESS;
}

COORD ct_get_win_charsize(HANDLE hcon)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hcon, &csbi);
    COORD w_charsize = {csbi.srWindow.Right - csbi.srWindow.Left + 1,
                        csbi.srWindow.Bottom - csbi.srWindow.Top + 1};

    return w_charsize;
}

F_STATUS ct_scroll_win_x(HANDLE hcon, SHORT offset)
{
    F_STATUS f_status;
    SMALL_RECT win_offset = {offset, 0, offset, 0};

    f_status = SetConsoleWindowInfo(hcon, FALSE, &win_offset);

    return (f_status == 0) ? F_ERROR : F_SUCCESS;
}

COORD ct_get_wbuffer_charsize(HANDLE hcon)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hcon, &csbi);
    COORD wbuffer_charsize = csbi.dwSize;

    return wbuffer_charsize;
}

F_STATUS ct_adjust_wbuffer(HANDLE hcon)
{
    F_STATUS f_status;

    COORD wbuffer_size = ct_get_win_charsize(hcon);
    f_status = SetConsoleScreenBufferSize(hcon, wbuffer_size);

    if (f_status == 0)
        return F_ERROR;

    SMALL_RECT wbuffer_offset = {-1, 0, -1, 0};
    f_status = SetConsoleWindowInfo(hcon, FALSE, &wbuffer_offset);

    return (f_status == 0) ? F_ERROR : F_SUCCESS;
}

COORD ct_add_wbuffer_x(HANDLE hcon, SHORT value)
{
    F_STATUS f_status;

    COORD w_buffersize = ct_get_wbuffer_charsize(hcon);
    w_buffersize.X += value;

    f_status = SetConsoleScreenBufferSize(hcon, w_buffersize);

    if (f_status != F_SUCCESS)
        w_buffersize.X -= value;

    return w_buffersize;
}

int ct_ievent_get_keypressed(BOOL *special_key)
{
    CTK key = _getch();

    if (key == CTK_FUNCTION_FLAG || key == CTK_ARROW_FLAG || key == CTK_BACKSPACE || key == CTK_ESC)
    {
        if (special_key != NULL)
            *special_key = TRUE;

        if (key == CTK_FUNCTION_FLAG || key == CTK_ARROW_FLAG)
            key = _getch();
    }
    else if (special_key != NULL)
        *special_key = FALSE;

    return key;
}

F_STATUS ct_hide_cursor(HANDLE hcon)
{
    F_STATUS f_status;
    CONSOLE_CURSOR_INFO cci;

    cci.dwSize = 100;
    cci.bVisible = FALSE;
    f_status = SetConsoleCursorInfo(hcon, &cci);

    return (f_status == 0) ? F_ERROR : F_SUCCESS;
}

F_STATUS ct_show_cursor(HANDLE hcon)
{
    F_STATUS f_status;
    CONSOLE_CURSOR_INFO cci;

    cci.dwSize = 100;
    cci.bVisible = TRUE;
    f_status = SetConsoleCursorInfo(hcon, &cci);

    return (f_status == 0) ? F_ERROR : F_SUCCESS;
}

void ct_clear_conw()
{
    system("cls");
}

COORD ct_get_cursor_position(HANDLE hcon)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    return GetConsoleScreenBufferInfo(hcon, &csbi) ? csbi.dwCursorPosition : INVALID_COORD;
}

COORD ct_set_cursor_position(HANDLE hcon, COORD new_cursor_pos)
{
    COORD wbuffer_charsize;
    COORD cursor_pos = ct_get_cursor_position(hcon);

    wbuffer_charsize = ct_get_wbuffer_charsize(hcon);

    if (new_cursor_pos.X < wbuffer_charsize.X && new_cursor_pos.Y < wbuffer_charsize.Y)
    {
        SetConsoleCursorPosition(hcon, new_cursor_pos);
        return new_cursor_pos;
    }

    return cursor_pos;
}

COORD ct_add_cursor_x(HANDLE hcon, int boundary, SHORT offset)
{
    COORD cursor_pos;

    cursor_pos = ct_get_cursor_position(hcon);
    cursor_pos.X += offset;

    if (cursor_pos.X >= 0 && cursor_pos.X <= boundary)
    {
        SetConsoleCursorPosition(hcon, cursor_pos);
        return cursor_pos;
    }

    cursor_pos.X -= offset;
    return cursor_pos;
}

COORD ct_add_cursor_y(HANDLE hcon, int boundary, SHORT offset)
{
    COORD cursor_pos;

    cursor_pos = ct_get_cursor_position(hcon);
    cursor_pos.Y += offset;

    if (cursor_pos.Y >= 0 && cursor_pos.Y <= boundary)
    {
        SetConsoleCursorPosition(hcon, cursor_pos);
        return cursor_pos;
    }

    cursor_pos.Y -= offset;
    return cursor_pos;
}

void ct_con_erase_char(HANDLE hcon)
{
    COORD cursor_pos = ct_get_cursor_position(hcon);

    if (cursor_pos.X > 0)
    {
        cursor_pos.X -= 1;

        SetConsoleCursorPosition(hcon, cursor_pos);
        putchar(' ');
        SetConsoleCursorPosition(hcon, cursor_pos);
    }
}

#endif // CT_CONWND_H
