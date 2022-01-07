#pragma once

#include <efi/types.h>

#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID                                           \
    {                                                                                  \
        0x387477c2, 0x69c7, 0x11d2, { 0x82, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

#define EFI_BLACK 0x0
#define EFI_BLUE (1 << 0)
#define EFI_GREEN (1 << 1)
#define EFI_CYAN (EFI_BLUE | EFI_GREEN)
#define EFI_RED (1 << 2)
#define EFI_MAGENTA (EFI_BLUE | EFI_RED)
#define EFI_BROWN (EFI_GREEN | EFI_RED)
#define EFI_LIGHTGRAY (EFI_BLUE | EFI_GREEN | EFI_RED)
#define EFI_BRIGHT (1 << 3)
#define EFI_DARKGRAY (EFI_BRIGHT)
#define EFI_LIGHTBLUE (EFI_BLUE | EFI_BRIGHT)
#define EFI_LIGHTGREEN (EFI_GREEN | EFI_BRIGHT)
#define EFI_LIGHTCYAN (EFI_CYAN | EFI_BRIGHT)
#define EFI_LIGHTRED (EFI_RED | EFI_BRIGHT)
#define EFI_LIGHTMAGENTA (EFI_MAGENTA | EFI_BRIGHT)
#define EFI_YELLOW (EFI_BROWN | EFI_BRIGHT)
#define EFI_WHITE (EFI_BLUE | EFI_GREEN | EFI_RED | EFI_BRIGHT)

#define EFI_BACKGROUND_BLACK 0x0
#define EFI_BACKGROUND_BLUE (1 << 4)
#define EFI_BACKGROUND_GREEN (1 << 5)
#define EFI_BACKGROUND_CYAN (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN)
#define EFI_BACKGROUND_RED (1 << 6)
#define EFI_BACKGROUND_MAGENTA (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_RED)
#define EFI_BACKGROUND_BROWN (EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)
#define EFI_BACKGROUND_LIGHTGRAY (EFI_BACKGROUND_BLUE | EFI_BACKGROUND_GREEN | EFI_BACKGROUND_RED)

struct _EFI_SIMPLE_TEXT_OUTPUT;

#define DEF_TEXTO_EFI_FUNC(name, ...) typedef EfiStatus (*EFI_TEXTO_##name)(struct _EFI_SIMPLE_TEXT_OUTPUT * self IFN(__VA_ARGS__)(, ) __VA_ARGS__)

DEF_TEXTO_EFI_FUNC(RESET, bool verification);
DEF_TEXTO_EFI_FUNC(OUTPUT_STRING, uint16_t *string);
DEF_TEXTO_EFI_FUNC(TEST_STRING, uint16_t *string);
DEF_TEXTO_EFI_FUNC(QUERY_MODE, uint32_t mode_number, uint32_t *columns, uint32_t *rows);
DEF_TEXTO_EFI_FUNC(SET_MODE, uint32_t mode_number);
DEF_TEXTO_EFI_FUNC(SET_ATTRIBUTE, uint32_t attribute);
DEF_TEXTO_EFI_FUNC(CLEAR_SCREEN);
DEF_TEXTO_EFI_FUNC(SET_CURSOR_POSITION, uint32_t column, uint32_t row);
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
