#pragma once

#include <efi/types.h>

#define CHAR_CARRIAGE_RETURN 0x000D

#define SCAN_NULL 0x0
#define SCAN_UP 0x1
#define SCAN_DOWN 0x2
#define SCAN_RIGHT 0x3
#define SCAN_LEFT 0x4
#define SCAN_HOME 0x5
#define SCAN_END 0x6
#define SCAN_INSERT 0x7
#define SCAN_DELETE 0x8
#define SCAN_PAGE_UP 0x9
#define SCAN_PAGE_DOWN 0xA
#define SCAN_F1 0xB
#define SCAN_F2 0xC
#define SCAN_F3 0xD
#define SCAN_F4 0xE
#define SCAN_F5 0xF
#define SCAN_F6 0x10
#define SCAN_F7 0x11
#define SCAN_F8 0x12
#define SCAN_F9 0x13
#define SCAN_F10 0x14
#define SCAN_F11 0x15
#define SCAN_F12 0x16
#define SCAN_ESC 0x17

#define EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID                                            \
    {                                                                                  \
        0x387477c1, 0x69c7, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

struct _EFI_SIMPLE_TEXT_INPUT;

#define DEF_TEXTI_EFI_FUNC(name, ...) typedef EfiStatus (*EFI_TEXTI_##name)(struct _EFI_SIMPLE_TEXT_INPUT * self IFN(__VA_ARGS__)(, ) __VA_ARGS__)

typedef struct
{
    uint16_t scan_code;
    uint16_t unicode_char;
} EFIInputKey;

DEF_TEXTI_EFI_FUNC(RESET, bool);
DEF_TEXTI_EFI_FUNC(READ_KEY, EFIInputKey *key);

typedef struct _EFI_SIMPLE_TEXT_INPUT
{
    EFI_TEXTI_RESET reset;
    EFI_TEXTI_READ_KEY read_key;
    EFIEvent wait_for_key;
} EFISimpleTextInput;
