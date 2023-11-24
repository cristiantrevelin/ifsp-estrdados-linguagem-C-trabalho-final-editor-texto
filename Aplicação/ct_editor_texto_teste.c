#include "headers/ct_pteditor.h"

void main()
{
    int c;

    HANDLE screen;
    HWND console_window;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT window;

    screen = GetStdHandle(STD_OUTPUT_HANDLE);
    console_window = GetConsoleWindow();

    DWORD dwWidth = GetSystemMetrics(SM_CXSCREEN);
    DWORD dwHeight = GetSystemMetrics(SM_CYSCREEN);

    // 500 x 500
    // 600 x 600
    // 800 x 600
    // MAXIMIZED

    ct_format_win(console_window, "CT - PLAIN TEXT EDITOR", 500, 500, TRUE);

    GetConsoleScreenBufferInfo(screen, &csbi);

    HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD irecord;
    WORD key;
    BOOL special_key;

    COORD t = ct_get_win_charsize(screen);
    int x = 3;
    x -= -1;
    printf("%d", x);

    do
    {
        c = ct_ievent_get_keypressed(&special_key);

        if (c == CTK_DOWN_ARROW)
        {
            GetConsoleScreenBufferInfo(screen, &csbi);
            t.X = csbi.dwCursorPosition.X - 1;
            t.Y = csbi.dwCursorPosition.Y;

            SetConsoleCursorPosition(screen, t);
            printf(" ");
            SetConsoleCursorPosition(screen, t);
        }
        else
            printf("%c", c);

    } while (c != CTK_F1);
}
