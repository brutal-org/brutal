#pragma once
#include <efi/base.h>

/* ---------- output -------------- */
#define EFI_BLACK 0x00
#define EFI_BLUE 0x01
#define EFI_GREEN 0x02
#define EFI_CYAN (EFI_BLUE | EFI_GREEN)
#define EFI_RED 0x04
#define EFI_MAGENTA (EFI_BLUE | EFI_RED)
#define EFI_BROWN (EFI_GREEN | EFI_RED)
#define EFI_LIGHTGRAY (EFI_BLUE | EFI_GREEN | EFI_RED)
#define EFI_BRIGHT 0x08
#define EFI_DARKGRAY (EFI_BRIGHT)
#define EFI_LIGHTBLUE (EFI_BLUE | EFI_BRIGHT)
#define EFI_LIGHTGREEN (EFI_GREEN | EFI_BRIGHT)
#define EFI_LIGHTCYAN (EFI_CYAN | EFI_BRIGHT)
#define EFI_LIGHTRED (EFI_RED | EFI_BRIGHT)
#define EFI_LIGHTMAGENTA (EFI_MAGENTA | EFI_BRIGHT)
#define EFI_YELLOW (EFI_BROWN | EFI_BRIGHT)
#define EFI_WHITE (EFI_BLUE | EFI_GREEN | EFI_RED | EFI_BRIGHT)

#define EFI_BACKGROUND_BLACK 0x00
#define EFI_BACKGROUND_BLUE 0x10
#define EFI_BACKGROUND_GREEN 0x20
#define EFI_BACKGROUND_CYAN (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN)
#define EFI_BACKGROUND_RED 0x40
#define EFI_BACKGROUND_MAGENTA (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_RED)
#define EFI_BACKGROUND_BROWN (EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)
#define EFI_BACKGROUND_LIGHTGRAY (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)

struct _EFI_SIMPLE_TEXT_OUTPUT;

#define DEF_TEXTO_EFI_FUNC(name, ...) typedef EFIStatus (*EFI_TEXTO_##name)(struct _EFI_SIMPLE_TEXT_OUTPUT * self __VA_OPT__(, ) __VA_ARGS__)

DEF_TEXTO_EFI_FUNC(RESET, bool verification);
DEF_TEXTO_EFI_FUNC(OUTPUT_STRING, char16 *string);
DEF_TEXTO_EFI_FUNC(TEST_STRING, char16 *string);
DEF_TEXTO_EFI_FUNC(QUERY_MODE, u32 mode_number, u32 *columns, u32 *rows);
DEF_TEXTO_EFI_FUNC(SET_MODE, u32 mode_number);
DEF_TEXTO_EFI_FUNC(SET_ATTRIBUTE, u32 attribute);
DEF_TEXTO_EFI_FUNC(CLEAR_SCREEN);
DEF_TEXTO_EFI_FUNC(SET_CURSOR_POSITION, u32 column, u32 row);
DEF_TEXTO_EFI_FUNC(ENABLE_CURSOR, bool visible);

typedef struct
{
    int32_t max_mode;
    int32_t mode;
    int32_t attribute;
    int32_t cursor_column;
    int32_t cursor_row;
    bool cursor_visible;
} SimpleTextOutputMode;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT
{
    EFI_TEXTO_RESET reset;
    EFI_TEXTO_OUTPUT_STRING output_string;
    EFI_TEXTO_TEST_STRING test_string;
    EFI_TEXTO_QUERY_MODE query_mode;
    EFI_TEXTO_SET_MODE set_mode;
    EFI_TEXTO_SET_ATTRIBUTE set_attribute;
    EFI_TEXTO_CLEAR_SCREEN clear_screen;
    EFI_TEXTO_SET_CURSOR_POSITION set_cursor_position;
    EFI_TEXTO_ENABLE_CURSOR enable_cursor;
    SimpleTextOutputMode *mode;

} EFISimpleTextOutput;

/* -------------- input --------------- */
#define CHAR_CARRIAGE_RETURN            0x000D
#define SCAN_NULL 0x0000
#define SCAN_UP 0x0001
#define SCAN_DOWN 0x0002
#define SCAN_RIGHT 0x0003
#define SCAN_LEFT 0x0004
#define SCAN_HOME 0x0005
#define SCAN_END 0x0006
#define SCAN_INSERT 0x0007
#define SCAN_DELETE 0x0008
#define SCAN_PAGE_UP 0x0009
#define SCAN_PAGE_DOWN 0x000A
#define SCAN_F1 0x000B
#define SCAN_F2 0x000C
#define SCAN_F3 0x000D
#define SCAN_F4 0x000E
#define SCAN_F5 0x000F
#define SCAN_F6 0x0010
#define SCAN_F7 0x0011
#define SCAN_F8 0x0012
#define SCAN_F9 0x0013
#define SCAN_F10 0x0014
#define SCAN_F11 0x0015
#define SCAN_F12 0x0016
#define SCAN_ESC 0x0017

#define EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID                                            \
    {                                                                                  \
        0x387477c1, 0x69c7, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

struct _EFI_SIMPLE_TEXT_INPUT;

#define DEF_TEXTI_EFI_FUNC(name, ...) typedef EFIStatus (*EFI_TEXTI_##name)(struct _EFI_SIMPLE_TEXT_INPUT * self __VA_OPT__(, ) __VA_ARGS__)

typedef struct
{
    u16 scan_code;
    u16 unicode_char;
} EFIInputKey;

DEF_TEXTI_EFI_FUNC(RESET, bool);
DEF_TEXTI_EFI_FUNC(READ_KEY, EFIInputKey *key);

typedef struct _EFI_SIMPLE_TEXT_INPUT
{
    EFI_TEXTI_RESET reset;
    EFI_TEXTI_READ_KEY read_key;
    EFIEvent wait_for_key;
} EFISimpleTextInput;
