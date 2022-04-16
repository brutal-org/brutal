#pragma once

#include <brutal-gfx>

#define UI_COLOR_DECL(NAME, VALUE) \
    static const GfxColor UI_COLOR_##NAME = gfx_hex(VALUE);

UI_COLOR_DECL(LIGHT_RED, 0xFB7185)
UI_COLOR_DECL(LIGHT_BLUE, 0x60A5FA)
UI_COLOR_DECL(LIGHT_GREEN, 0x4ADE80)
UI_COLOR_DECL(LIGHT_ORANGE, 0xFB923C)
UI_COLOR_DECL(ORANGE, 0xF97316)
UI_COLOR_DECL(RED, 0xEF4444)
UI_COLOR_DECL(BLUE, 0x3B82F6)
UI_COLOR_DECL(GREEN, 0x22C55E)

UI_COLOR_DECL(BASE00, 0x18181B)
UI_COLOR_DECL(BASE01, 0x27272A)
UI_COLOR_DECL(BASE02, 0x3F3F46)
UI_COLOR_DECL(BASE03, 0x52525B)
UI_COLOR_DECL(BASE04, 0x71717A)
UI_COLOR_DECL(BASE05, 0xA1A1AA)
UI_COLOR_DECL(BASE06, 0xD4D4D8)
UI_COLOR_DECL(BASE07, 0xE4E4E7)
UI_COLOR_DECL(BASE08, 0xF4F4F5)
UI_COLOR_DECL(BASE09, 0xFAFAFA)

typedef enum
{
    UI_COLOR_ROLE_TEXT,
    UI_COLOR_ROLE_BODY,
    UI_COLOR_ROLE_ACCENT,
    UI_COLOR_ROLE_BORDER,
    UI_COLOR_ROLE_PANEL,

    __UI_COLOR_ROLE_COUNT,
} UiRole;

typedef struct
{
    bool present;
    GfxColor color;
} UiColor;

typedef struct
{
    UiColor colors[__UI_COLOR_ROLE_COUNT];
} UiPalette;

void ui_palette_init(UiPalette *palette);

bool ui_palette_color(UiPalette *palette, UiRole role, GfxColor *color);

void ui_palette_overide(UiPalette *palette, UiRole role, GfxColor color);
