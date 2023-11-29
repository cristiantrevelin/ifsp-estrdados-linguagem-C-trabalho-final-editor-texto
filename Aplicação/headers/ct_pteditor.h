#ifndef CT_PTEDITOR_H
#define CT_PTEDITOR_H

/// PLAIN TEXT EDITOR - EDITING FUNCTIONS HEADER.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <locale.h>

#include "ct_definitions.h"
#include "ct_page.h"
#include "ct_conwnd.h"

#define CONFIGW_WIDTH 310
#define CONFIGW_HEIGHT 210
#define CONFIGW_TITLE "CT - PTE CONFIG"

#define EDITORW_TITLE "CT - PLAIN TEXT EDITOR"

typedef enum eWDimension
{
    WD_5X5 = 3,
    WD_6X6,
    WD_8X6

} WDimension;


COORD ct_pte_move_cursor_x(HANDLE hcon, ROW_NODE *focus_row, X_DIRECTION direction)
{
    COORD cursor_pos;

    if (focus_row == NULL)
        return INVALID_COORD;

    switch(direction)
    {
        case D_LEFT:
            cursor_pos = ct_add_cursor_x(hcon, ct_get_row_length(focus_row), -1);
            break;

        default:
            cursor_pos = ct_add_cursor_x(hcon, ct_get_row_length(focus_row), 1);
            break;
    }

    return cursor_pos;
}

COORD ct_pte_move_cursor_y(HANDLE hcon, PAGE *pptr, ROW_NODE *focus_row, Y_DIRECTION direction)
{
    COORD cursor_pos;
    int row_length;

    if (pptr == NULL)
        return INVALID_COORD;

    switch(direction)
    {
        case D_UP:
            cursor_pos = ct_add_cursor_y(hcon, ct_get_page_length(pptr) - 1, -1);
            break;

        default:
            cursor_pos = ct_add_cursor_y(hcon, ct_get_page_length(pptr) - 1, 1);
            break;
    }

    focus_row = ct_get_focus_row(pptr, cursor_pos);
    row_length = ct_get_row_length(focus_row);

    if (cursor_pos.X > row_length)
        ct_add_cursor_x(hcon, row_length, row_length - cursor_pos.X);

    return cursor_pos;
}

BOOL ct_pte_cursor_at_row_tail(ROW_NODE *focus_row, COORD cursor_pos)
{
    return (cursor_pos.X == ct_get_row_length(focus_row) - 1) ? TRUE : FALSE;
}

void ct_pte_rearrange_rows(PAGE *pptr, ROW_NODE *start_row)
{
    F_STATUS f_status;
    ROW_NODE *aux_row = start_row;

    f_status = ct_rearrange_row_last_char(aux_row -> row_buffer, aux_row -> next -> row_buffer);

    if (f_status == F_FULL_BUFFER)
    {
        ct_pte_rearrange_rows(pptr, aux_row -> next);
        ct_rearrange_row_last_char(aux_row -> row_buffer, aux_row -> next -> row_buffer);
    }
}

void ct_pte_push_row_char(PAGE *pptr, ROW_NODE *focus_row, char key_input, COORD cursor_pos)
{
    F_STATUS f_status;
    ROW_NODE *aux_row = focus_row;

    f_status = ct_pushp_row_char(aux_row -> row_buffer, key_input, cursor_pos.X);

    if (f_status == F_FULL_BUFFER)
    {
        ct_pte_rearrange_rows(pptr, aux_row);
        ct_pushp_row_char(aux_row -> row_buffer, key_input, cursor_pos.X);
    }
}

void ct_pte_draw_row_from_cursor_pos(HANDLE hcon, ROW_NODE *focus_row, COORD cursor_pos)
{
    ROW_NODE *aux_row = focus_row;

    for (int i = cursor_pos.X; i < ct_get_row_length(aux_row); i++)
        putchar(ct_get_row_char(aux_row, i));

    cursor_pos.X++;
    ct_set_cursor_position(hcon, cursor_pos);
}

void ct_pte_draw_rows_from_cursor_pos(HANDLE hcon, ROW_NODE *focus_row, COORD cursor_pos)
{
    ROW_NODE *aux_row = focus_row;
    COORD aux_cursor_pos = cursor_pos;

    while (aux_row != NULL)
    {
        ct_pte_draw_row_from_cursor_pos(hcon, aux_row, cursor_pos);

        aux_row = aux_row -> next;
        aux_cursor_pos = ct_get_cursor_position(hcon);

        cursor_pos.X = 0;
        cursor_pos.Y++;
    }

    ct_set_cursor_position(hcon, aux_cursor_pos);
}

void ct_repaint_screen(HANDLE hcon, PAGE *pptr, COORD cursor_pos)
{
    system("cls");

    ROW_NODE *aux_row = pptr -> first_row;
    COORD aux_cursor_pos = cursor_pos;

    for (int i = 0; i < pptr -> length; i++)
    {
        for (int j = 0; j <= aux_row -> row_buffer -> n; j++)
            putchar(aux_row -> row_buffer -> buffer[j]);

        aux_row = aux_row -> next;
    }

    cursor_pos.X++;
    ct_set_cursor_position(hcon, cursor_pos);
}



/// MAIN EXECUTION FUNCTIONS:

int ct_playpte_configw()
{
    F_STATUS f_status;
    HWND hwnd = ct_get_conw();
    HANDLE hcon = ct_get_wbuffer();

    if (hwnd == INVALID_HANDLE_VALUE || hcon == INVALID_HANDLE_VALUE)
        return -1;

    f_status = ct_format_win(hwnd, CONFIGW_TITLE, CONFIGW_WIDTH, CONFIGW_HEIGHT, TRUE);

    if (f_status != F_SUCCESS)
        return -1;

    ct_adjust_wbuffer(hcon);
    ct_hide_cursor(hcon);

    setlocale(LC_ALL, "Portuguese");

    printf("#--------------------------------#");
    printf(" SELECIONE AS DIMENSÕES DO EDITOR\n");
    printf("#--------------------------------#");
    printf("            500 x 500");
    printf("\n            600 x 600");
    printf("\n            800 x 600\n");
    printf("#--------------------------------#");
    printf("  PRESSIONE ENTER PARA ESCOLHER!\n");
    printf("#--------------------------------#");

    CTK input;
    COORD cursor_pos = {8, 3};

    SetConsoleCursorPosition(hcon, cursor_pos);
    printf(">>");

    do
    {
        input = ct_ievent_get_keypressed(NULL);

        if (input == CTK_DOWN_ARROW)
        {
            if (cursor_pos.Y < 5)
            {
                SetConsoleCursorPosition(hcon, cursor_pos);
                printf("  ");

                cursor_pos.Y++;
                SetConsoleCursorPosition(hcon, cursor_pos);
                printf(">>");
            }
        }
        else if (input == CTK_UP_ARROW)
        {
            if (cursor_pos.Y > 3)
            {
                SetConsoleCursorPosition(hcon, cursor_pos);
                printf("  ");

                cursor_pos.Y--;
                SetConsoleCursorPosition(hcon, cursor_pos);
                printf(">>");
            }
        }

    } while (input != CTK_CARRIAGE_RETURN);

    return cursor_pos.Y;
}

F_STATUS ct_play_plaintext_editor(WDimension WINDOW_DIMENSION)
{
    F_STATUS f_status;

    COORD WINDOW_DIMENSION_VALUES;
    COORD WINDOW_CHARSIZE;
    COORD wbuffer_charsize;
    HWND hwnd;
    HANDLE hcon;

    int ROW_BUFFER_LENGTH;

    hwnd = ct_get_conw();
    hcon = ct_get_wbuffer();


    switch(WINDOW_DIMENSION)
    {
        case WD_6X6:
            WINDOW_DIMENSION_VALUES.X = 600;
            WINDOW_DIMENSION_VALUES.Y = 600;
            break;

        case WD_8X6:
            WINDOW_DIMENSION_VALUES.X = 800;
            WINDOW_DIMENSION_VALUES.Y = 600;
            break;

        default:
            WINDOW_DIMENSION_VALUES.X = 500;
            WINDOW_DIMENSION_VALUES.Y = 500;
            break;
    }

    f_status = ct_format_win(hwnd, EDITORW_TITLE, WINDOW_DIMENSION_VALUES.X, WINDOW_DIMENSION_VALUES.Y, TRUE);

    if (f_status != F_SUCCESS)
        return F_ERROR;

    ct_adjust_wbuffer(hcon);

    WINDOW_CHARSIZE = ct_get_win_charsize(hcon);
    ROW_BUFFER_LENGTH = WINDOW_CHARSIZE.X;

    PAGE *page;
    ROW_NODE *focus_row;

    page = ct_create_page();

    if (page == NULL)
        return F_ERROR;

    ct_pushe_row(page, ROW_BUFFER_LENGTH);
    focus_row = page -> first_row;

    ct_clear_conw();
    ct_show_cursor(hcon);

    CTK key_input;
    BOOL is_special_key;
    COORD cursor_pos = ct_get_cursor_position(hcon);

    APP_STATE app_state;
    app_state = APS_RUNNING;

    setlocale(LC_ALL, "C");

    do
    {
        key_input = ct_ievent_get_keypressed(&is_special_key);

        if (is_special_key)
        {
            switch (key_input)
            {
                case CTK_ESC:
                    app_state = APS_EXIT;
                    break;

                case CTK_BACKSPACE:
                    ct_con_erase_char(hcon);
                    break;

                case CTK_LEFT_ARROW:
                    cursor_pos = ct_pte_move_cursor_x(hcon, focus_row, D_LEFT);
                    break;

                case CTK_RIGHT_ARROW:
                    cursor_pos = ct_pte_move_cursor_x(hcon, focus_row, D_RIGHT);
                    break;

                case CTK_UP_ARROW:
                    cursor_pos = ct_pte_move_cursor_y(hcon, page, focus_row, D_UP);
                    focus_row = ct_get_focus_row(page, cursor_pos);
                    break;

                case CTK_DOWN_ARROW:
                    cursor_pos = ct_pte_move_cursor_y(hcon, page, focus_row, D_DOWN);
                    focus_row = ct_get_focus_row(page, cursor_pos);
                    break;

                case CTK_F1:
                    break;
            }
        }
        else
        {
            if (key_input == CTK_CARRIAGE_RETURN)
            {
                ct_pushe_row(page, ROW_BUFFER_LENGTH);

                cursor_pos = ct_get_cursor_position(hcon);
                cursor_pos = ct_pte_move_cursor_y(hcon, page, focus_row, D_DOWN);

                focus_row = ct_get_focus_row(page, cursor_pos);
            }
            else
            {
                cursor_pos = ct_get_cursor_position(hcon);
                ct_pte_push_row_char(page, focus_row, key_input, cursor_pos);

                ct_repaint_screen(hcon, page, cursor_pos);
                //ct_pte_draw_rows_from_cursor_pos(hcon, focus_row, cursor_pos);

                if (ct_full_row_buffer(page -> last_row -> row_buffer))
                {
                    ct_pushe_row(page, ROW_BUFFER_LENGTH);

                    if (ct_pte_cursor_at_row_tail(focus_row, cursor_pos))
                    {
                        cursor_pos = ct_set_cursor_position(hcon, (COORD) {0, cursor_pos.Y + 1});
                        focus_row = ct_get_focus_row(page, cursor_pos);
                    }
                }
            }
        }

    } while(app_state != APS_EXIT);
}

#endif ///CT_PTEDITOR_H
