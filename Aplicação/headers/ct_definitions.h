#ifndef CT_DEFINITIONS_H
#define CT_DEFINITIONS_H

// CONVENTIONS FOR CT HEADERS.


// ------------------------ APPLICATION STATES -----------------------

typedef enum eAppState
{
    APS_EXIT = 0,
    APS_RUNNING

} APP_STATE;



// ------------------------ GENERIC DIRECTIONS -----------------------

typedef enum eXDirection
{
    D_LEFT = 0,
    D_RIGHT

} X_DIRECTION;

typedef enum eYDirection
{
    D_UP = 0,
    D_DOWN

} Y_DIRECTION;



// ---------------------------- INPUT KEYS ---------------------------
// KEY VALUES REGARDING ASCII TABLE:
// CTK : CT Key Type

typedef int CTK;

#define CTK_FUNCTION_FLAG 0
#define CTK_ARROW_FLAG 0xe0

#define CTK_ESC 0x1b
#define CTK_F1 0x03b
#define CTK_CARRIAGE_RETURN 0xd
#define CTK_EOL 0xd0a
#define CTK_BACKSPACE 0x8
#define CTK_LEFT_ARROW 0x4b
#define CTK_RIGHT_ARROW 0x4d
#define CTK_DOWN_ARROW 0x50
#define CTK_UP_ARROW 0x48



// ---------------------------- FUNCTIONS ----------------------------
// NAMING FUNCTIONS: <header label>_<extra label - optional>_<action - verb>_<object - noun>_<extra info - object>
//     - ex: ct_set_win_size( args )
//     - ex: ct_pte_move_cursor( args )

// FUNCTION RETURNING STATUS:

typedef short int F_STATUS;

#define F_SUCCESS 0

#define F_ERROR -1
#define F_ALLOCATION_ERROR -2
#define F_FULL_BUFFER -3
#define F_EMPTY_BUFFER -4
#define F_INVALID_INDEX -5



// ------------------------ INVALID LITERALS -------------------------

#define INVALID_CHAR (char) '\0'
#define INVALID_COORD (COORD) {-1, -1}

// ----------------------------- MASKS -------------------------------

#define MSB_CHECK_MASK 0x8000


#endif // CT_DEFINITIONS_H
