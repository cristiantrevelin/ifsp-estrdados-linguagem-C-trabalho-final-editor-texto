#ifndef CT_PAGE_H
#define CT_PAGE_H

// PLAIN TEXT EDITOR - PAGE STRUCTURE HEADER.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#include "ct_definitions.h"


// MAIN STRUCTURES:

typedef struct sRowBuffer
{
    char *buffer;
    int max_length;
    int n;

} ROW_BUFFER;

typedef struct sRowNode
{
    ROW_BUFFER *row_buffer;
    struct sRowNode *next;
    struct sRowNode *previous;

} ROW_NODE;

typedef struct sPage
{
    ROW_NODE *first_row;
    ROW_NODE *last_row;
    SHORT length;

} PAGE;


ROW_BUFFER *ct_allocate_row_buffer()
{
    return (ROW_BUFFER *) malloc(sizeof(ROW_BUFFER));
}

F_STATUS ct_init_row_buffer(ROW_BUFFER *rbptr, int buffer_length)
{
    rbptr -> buffer = (char *) malloc(sizeof(char) * buffer_length);
    rbptr -> max_length = buffer_length;
    rbptr -> n = -1;

    return (rbptr -> buffer == NULL) ? F_ALLOCATION_ERROR : F_SUCCESS;
}

ROW_BUFFER *ct_create_row_buffer(int buffer_length)
{
    F_STATUS f_status;
    ROW_BUFFER *rbptr = ct_allocate_row_buffer();

    if (rbptr == NULL)
        return NULL;

    f_status = ct_init_row_buffer(rbptr, buffer_length);

    if (f_status != F_SUCCESS)
    {
        free(rbptr);
        return NULL;
    }

    return rbptr;
}

ROW_NODE *ct_allocate_row()
{
    return (ROW_NODE *) malloc(sizeof(ROW_NODE));
}

F_STATUS ct_init_row(ROW_NODE *rptr, int buffer_length)
{
    rptr -> row_buffer = ct_create_row_buffer(buffer_length);
    rptr -> next = NULL;
    rptr -> previous = NULL;

    return (rptr -> row_buffer == NULL) ? F_ALLOCATION_ERROR : F_SUCCESS;
}

ROW_NODE *ct_create_row(int buffer_length)
{
    F_STATUS f_status;
    ROW_NODE *rptr = ct_allocate_row();

    if (rptr == NULL)
        return NULL;

    f_status = ct_init_row(rptr, buffer_length);

    return (f_status != F_SUCCESS) ? NULL : rptr;
}

PAGE *ct_allocate_page()
{
    return (PAGE *) malloc(sizeof(PAGE));
}

void ct_init_page(PAGE *pptr)
{
    pptr -> first_row = NULL;
    pptr -> last_row = NULL;
    pptr -> length = 0;
}

PAGE *ct_create_page()
{
    PAGE *pptr = ct_allocate_page();

    if (pptr == NULL)
        return NULL;

    ct_init_page(pptr);

    return pptr;
}

BOOL ct_empty_row_buffer(ROW_BUFFER *rbptr)
{
    return (rbptr -> n == -1) ? TRUE : FALSE;
}

BOOL ct_full_row_buffer(ROW_BUFFER *rbptr)
{
    return (rbptr -> n == rbptr -> max_length - 1) ? TRUE : FALSE;
}

BOOL ct_last_row(ROW_NODE *rptr)
{
    return (rptr -> next == NULL) ? TRUE : FALSE;
}

int ct_get_row_length(ROW_NODE *rptr)
{
    return rptr -> row_buffer -> n + 1;
}

int ct_get_row_buffer_length(ROW_BUFFER *rbptr)
{
    return rbptr -> n + 1;
}

int ct_get_row_max_length(ROW_NODE *rptr)
{
    return rptr -> row_buffer -> max_length;
}

char ct_get_row_char(ROW_NODE *rptr, int index)
{
    return rptr -> row_buffer -> buffer[index];
}

BOOL ct_empty_page(PAGE *pptr)
{
    return (pptr -> length == 0) ? TRUE : FALSE;
}

int ct_get_page_length(PAGE *pptr)
{
    return pptr -> length;
}

F_STATUS ct_pushe_row_char(ROW_BUFFER *rbptr, char value)
{
    if (ct_full_row_buffer(rbptr))
        return F_FULL_BUFFER;

    (rbptr -> n)++;
    rbptr -> buffer[rbptr -> n] = value;

    return F_SUCCESS;
}

F_STATUS ct_pushp_row_char(ROW_BUFFER *rbptr, char value, int pos)
{
    if (ct_full_row_buffer(rbptr))
        return F_FULL_BUFFER;

    if (pos < 0 || pos > rbptr -> n + 1)
        return F_INVALID_INDEX;

    (rbptr -> n)++;

    for (int i = rbptr -> n; i > pos; i--)
        rbptr -> buffer[i] = rbptr -> buffer[i - 1];

    rbptr -> buffer[pos] = value;

    return F_SUCCESS;
}

F_STATUS ct_pushe_row(PAGE *pptr, int buffer_length)
{
    ROW_NODE *new_row = ct_create_row(buffer_length);

    if (new_row == NULL)
        return F_ALLOCATION_ERROR;

    if (ct_empty_page(pptr))
    {
        pptr -> first_row = new_row;
        pptr -> last_row = new_row;
    }
    else
    {
        new_row -> previous = pptr -> last_row;
        pptr -> last_row -> next = new_row;
        pptr -> last_row = new_row;
    }

    (pptr -> length)++;

    return F_SUCCESS;
}

F_STATUS ct_pusha_row(PAGE *pptr, ROW_NODE *rptr, int buffer_length)
{
    ROW_NODE *new_row = ct_create_row(buffer_length);

    if (new_row == NULL)
        return F_ALLOCATION_ERROR;

    if (ct_last_row(rptr))
        pptr -> last_row = new_row;
    else
        rptr -> next -> previous = new_row;

    new_row -> next = rptr -> next;
    rptr -> next = new_row;
    new_row -> previous = rptr;

    (pptr -> length)++;

    return F_SUCCESS;
}

char ct_pope_row_char(ROW_BUFFER *rbptr)
{
    char value;

    if (ct_empty_row_buffer(rbptr))
        value = INVALID_CHAR;
    else
    {
        value = rbptr -> buffer[rbptr -> n];
        (rbptr -> n)--;
    }

    return value;
}

char ct_popp_row_char(ROW_BUFFER *rbptr, int pos)
{
    char value;

    if (ct_empty_row_buffer(rbptr))
        value = INVALID_CHAR;
    else
    {
        value = rbptr -> buffer[pos];
        (rbptr -> n)--;

        for (int i = pos; i <= rbptr -> n; i++)
            rbptr -> buffer[i] = rbptr -> buffer[i + 1];
    }

    return value;
}

ROW_NODE *ct_get_focus_row(PAGE *pptr, COORD cursor_pos)
{
    ROW_NODE *aux_search_row;
    SHORT offset;

    if ((SHORT) (cursor_pos.Y + 1) > pptr -> length)
        return NULL;

    if ((SHORT) (cursor_pos.Y + 1) > (pptr -> length) / 2)
    {
        aux_search_row = pptr -> last_row;
        offset = (pptr -> length) - cursor_pos.Y - (SHORT) 1;

        for (SHORT i = 0; i < offset; i++)
            aux_search_row = aux_search_row -> previous;
    }
    else
    {
        aux_search_row = pptr -> first_row;
        offset = cursor_pos.Y;

        for (SHORT i = 0; i < offset; i++)
            aux_search_row = aux_search_row -> next;
    }

    return aux_search_row;
}

F_STATUS ct_rearrange_row_last_char(ROW_BUFFER *source_rbptr, ROW_BUFFER *receiver_rbptr)
{
    char value;

    if (ct_empty_row_buffer(source_rbptr))
        return F_EMPTY_BUFFER;

    if (ct_full_row_buffer(receiver_rbptr))
        return F_FULL_BUFFER;

    value = ct_pope_row_char(source_rbptr);
    ct_pushp_row_char(receiver_rbptr, value, 0);

    return F_SUCCESS;
}

int ct_rearrange_row_chars(ROW_BUFFER *source_rbptr, ROW_BUFFER *receiver_rbptr, int start_pos)
{
    char value;
    int source_length;
    int rearranged_chars = 0;

    if (ct_empty_row_buffer(source_rbptr))
        return -1;

    if (ct_full_row_buffer(receiver_rbptr))
        return -1;

    source_length = source_rbptr -> n + 1;

    for (int i = start_pos; i < source_length; i++)
    {
        value = ct_popp_row_char(source_rbptr, start_pos);
        ct_pushe_row_char(receiver_rbptr, value);

        rearranged_chars++;
    }

    return rearranged_chars;
}

#endif // CT_PAGE_H
