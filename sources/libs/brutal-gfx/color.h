#pragma once

#include <brutal-base>

typedef struct
{
    uint8_t r, g, b, a;
} GfxColor;

#define gfx_rgba(R, G, B, A) ((GfxColor){(R), (G), (B), (A)})
#define gfx_rgb(R, G, B) gfx_rgba(R, G, B, 255)
#define gfx_hex(HEX) gfx_rgba((uint8_t)(HEX >> 16), (uint8_t)(HEX >> 8), (uint8_t)HEX, 255)

#define GFX_FOREACH_COLOR(COLOR)             \
    COLOR(ALICE_BLUE, 0xF0F8FF)              \
    COLOR(ANTIQUE_WHITE, 0xFAEBD7)           \
    COLOR(AQUA, 0x00FFFF)                    \
    COLOR(AQUAMARINE, 0x7FFFD4)              \
    COLOR(AZURE, 0xF0FFFF)                   \
    COLOR(BEIGE, 0xF5F5DC)                   \
    COLOR(BISQUE, 0xFFE4C4)                  \
    COLOR(BLACK, 0x000000)                   \
    COLOR(BLANCHED_ALMOND, 0xFFEBCD)         \
    COLOR(BLUE, 0x0000FF)                    \
    COLOR(BLUE_VIOLET, 0x8A2BE2)             \
    COLOR(BROWN, 0xA52A2A)                   \
    COLOR(BURLY_WOOD, 0xDEB887)              \
    COLOR(CADET_BLUE, 0x5F9EA0)              \
    COLOR(CHARTREUSE, 0x7FFF00)              \
    COLOR(CHOCOLATE, 0xD2691E)               \
    COLOR(CORAL, 0xFF7F50)                   \
    COLOR(CORNFLOWER_BLUE, 0x6495ED)         \
    COLOR(CORNSILK, 0xFFF8DC)                \
    COLOR(CRIMSON, 0xDC143C)                 \
    COLOR(CYAN, 0x00FFFF)                    \
    COLOR(DARK_BLUE, 0x00008B)               \
    COLOR(DARK_CYAN, 0x008B8B)               \
    COLOR(DARK_GOLDEN_ROD, 0xB8860B)         \
    COLOR(DARK_GRAY, 0xA9A9A9)               \
    COLOR(DARK_GREY, 0xA9A9A9)               \
    COLOR(DARK_GREEN, 0x006400)              \
    COLOR(DARK_KHAKI, 0xBDB76B)              \
    COLOR(DARK_MAGENTA, 0x8B008B)            \
    COLOR(DARK_OLIVE_GREEN, 0x556B2F)        \
    COLOR(DARK_ORANGE, 0xFF8C00)             \
    COLOR(DARK_ORCHID, 0x9932CC)             \
    COLOR(DARK_RED, 0x8B0000)                \
    COLOR(DARK_SALMON, 0xE9967A)             \
    COLOR(DARK_SEA_GREEN, 0x8FBC8F)          \
    COLOR(DARK_SLATE_BLUE, 0x483D8B)         \
    COLOR(DARK_SLATE_GRAY, 0x2F4F4F)         \
    COLOR(DARK_SLATE_GREY, 0x2F4F4F)         \
    COLOR(DARK_TURQUOISE, 0x00CED1)          \
    COLOR(DARK_VIOLET, 0x9400D3)             \
    COLOR(DEEP_PINK, 0xFF1493)               \
    COLOR(DEEP_SKY_BLUE, 0x00BFFF)           \
    COLOR(DIM_GRAY, 0x696969)                \
    COLOR(DIM_GREY, 0x696969)                \
    COLOR(DODGER_BLUE, 0x1E90FF)             \
    COLOR(FIRE_BRICK, 0xB22222)              \
    COLOR(FLORAL_WHITE, 0xFFFAF0)            \
    COLOR(FOREST_GREEN, 0x228B22)            \
    COLOR(FUCHSIA, 0xFF00FF)                 \
    COLOR(GAINSBORO, 0xDCDCDC)               \
    COLOR(GHOST_WHITE, 0xF8F8FF)             \
    COLOR(GOLD, 0xFFD700)                    \
    COLOR(GOLDEN_ROD, 0xDAA520)              \
    COLOR(GRAY, 0x808080)                    \
    COLOR(GREY, 0x808080)                    \
    COLOR(GREEN, 0x008000)                   \
    COLOR(GREEN_YELLOW, 0xADFF2F)            \
    COLOR(HONEY_DEW, 0xF0FFF0)               \
    COLOR(HOT_PINK, 0xFF69B4)                \
    COLOR(INDIAN_RED, 0xCD5C5C)              \
    COLOR(INDIGO, 0x4B0082)                  \
    COLOR(IVORY, 0xFFFFF0)                   \
    COLOR(KHAKI, 0xF0E68C)                   \
    COLOR(LAVENDER, 0xE6E6FA)                \
    COLOR(LAVENDER_BLUSH, 0xFFF0F5)          \
    COLOR(LAWN_GREEN, 0x7CFC00)              \
    COLOR(LEMON_CHIFFON, 0xFFFACD)           \
    COLOR(LIGHT_BLUE, 0xADD8E6)              \
    COLOR(LIGHT_CORAL, 0xF08080)             \
    COLOR(LIGHT_CYAN, 0xE0FFFF)              \
    COLOR(LIGHT_GOLDEN_ROD_YELLOW, 0xFAFAD2) \
    COLOR(LIGHT_GRAY, 0xD3D3D3)              \
    COLOR(LIGHT_GREY, 0xD3D3D3)              \
    COLOR(LIGHT_GREEN, 0x90EE90)             \
    COLOR(LIGHT_PINK, 0xFFB6C1)              \
    COLOR(LIGHT_SALMON, 0xFFA07A)            \
    COLOR(LIGHT_SEA_GREEN, 0x20B2AA)         \
    COLOR(LIGHT_SKY_BLUE, 0x87CEFA)          \
    COLOR(LIGHT_SLATE_GRAY, 0x778899)        \
    COLOR(LIGHT_SLATE_GREY, 0x778899)        \
    COLOR(LIGHT_STEEL_BLUE, 0xB0C4DE)        \
    COLOR(LIGHT_YELLOW, 0xFFFFE0)            \
    COLOR(LIME, 0x00FF00)                    \
    COLOR(LIME_GREEN, 0x32CD32)              \
    COLOR(LINEN, 0xFAF0E6)                   \
    COLOR(MAGENTA, 0xFF00FF)                 \
    COLOR(MAROON, 0x800000)                  \
    COLOR(MEDIUM_AQUA_MARINE, 0x66CDAA)      \
    COLOR(MEDIUM_BLUE, 0x0000CD)             \
    COLOR(MEDIUM_ORCHID, 0xBA55D3)           \
    COLOR(MEDIUM_PURPLE, 0x9370DB)           \
    COLOR(MEDIUM_SEA_GREEN, 0x3CB371)        \
    COLOR(MEDIUM_SLATE_BLUE, 0x7B68EE)       \
    COLOR(MEDIUM_SPRING_GREEN, 0x00FA9A)     \
    COLOR(MEDIUM_TURQUOISE, 0x48D1CC)        \
    COLOR(MEDIUM_VIOLET_RED, 0xC71585)       \
    COLOR(MIDNIGHT_BLUE, 0x191970)           \
    COLOR(MINT_CREAM, 0xF5FFFA)              \
    COLOR(MISTY_ROSE, 0xFFE4E1)              \
    COLOR(MOCCASIN, 0xFFE4B5)                \
    COLOR(NAVAJO_WHITE, 0xFFDEAD)            \
    COLOR(NAVY, 0x000080)                    \
    COLOR(OLD_LACE, 0xFDF5E6)                \
    COLOR(OLIVE, 0x808000)                   \
    COLOR(OLIVE_DRAB, 0x6B8E23)              \
    COLOR(ORANGE, 0xFFA500)                  \
    COLOR(ORANGE_RED, 0xFF4500)              \
    COLOR(ORCHID, 0xDA70D6)                  \
    COLOR(PALE_GOLDEN_ROD, 0xEEE8AA)         \
    COLOR(PALE_GREEN, 0x98FB98)              \
    COLOR(PALE_TURQUOISE, 0xAFEEEE)          \
    COLOR(PALE_VIOLET_RED, 0xDB7093)         \
    COLOR(PAPAYA_WHIP, 0xFFEFD5)             \
    COLOR(PEACH_PUFF, 0xFFDAB9)              \
    COLOR(PERU, 0xCD853F)                    \
    COLOR(PINK, 0xFFC0CB)                    \
    COLOR(PLUM, 0xDDA0DD)                    \
    COLOR(POWDER_BLUE, 0xB0E0E6)             \
    COLOR(PURPLE, 0x800080)                  \
    COLOR(REBECCA_PURPLE, 0x663399)          \
    COLOR(RED, 0xFF0000)                     \
    COLOR(ROSY_BROWN, 0xBC8F8F)              \
    COLOR(ROYAL_BLUE, 0x4169E1)              \
    COLOR(SADDLE_BROWN, 0x8B4513)            \
    COLOR(SALMON, 0xFA8072)                  \
    COLOR(SANDY_BROWN, 0xF4A460)             \
    COLOR(SEA_GREEN, 0x2E8B57)               \
    COLOR(SEA_SHELL, 0xFFF5EE)               \
    COLOR(SIENNA, 0xA0522D)                  \
    COLOR(SILVER, 0xC0C0C0)                  \
    COLOR(SKY_BLUE, 0x87CEEB)                \
    COLOR(SLATE_BLUE, 0x6A5ACD)              \
    COLOR(SLATE_GRAY, 0x708090)              \
    COLOR(SLATE_GREY, 0x708090)              \
    COLOR(SNOW, 0xFFFAFA)                    \
    COLOR(SPRING_GREEN, 0x00FF7F)            \
    COLOR(STEEL_BLUE, 0x4682B4)              \
    COLOR(TAN, 0xD2B48C)                     \
    COLOR(TEAL, 0x008080)                    \
    COLOR(THISTLE, 0xD8BFD8)                 \
    COLOR(TOMATO, 0xFF6347)                  \
    COLOR(TURQUOISE, 0x40E0D0)               \
    COLOR(VIOLET, 0xEE82EE)                  \
    COLOR(WHEAT, 0xF5DEB3)                   \
    COLOR(WHITE, 0xFFFFFF)                   \
    COLOR(WHITE_SMOKE, 0xF5F5F5)             \
    COLOR(YELLOW, 0xFFFF00)                  \
    COLOR(YELLOW_GREEN, 0x9ACD32)

#define COLOR_DECL(NAME, VALUE) \
    static const GfxColor GFX_##NAME = (gfx_hex(VALUE));

GFX_FOREACH_COLOR(COLOR_DECL)

static inline GfxColor gfx_blend(GfxColor fg, GfxColor bg)
{
    if (fg.a == 0xff)
    {
        return fg;
    }
    else if (fg.a == 0)
    {
        return bg;
    }
    else
    {
        uint16_t d = 255u * (bg.a + fg.a) - bg.a * fg.a;

        return gfx_rgba(
            (bg.r * bg.a * (255u - fg.a) + 255u * fg.a * fg.r) / d,
            (bg.g * bg.a * (255u - fg.a) + 255u * fg.a * fg.g) / d,
            (bg.b * bg.a * (255u - fg.a) + 255u * fg.a * fg.b) / d,
            d / 255u);
    }
}

static inline GfxColor gfx_lerp(GfxColor a, GfxColor b, float t)
{
    return gfx_rgba(
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t);
}

GfxColor gfx_color_rand(uint8_t alpha);

static inline GfxColor gfx_color_with_alpha(GfxColor color, uint8_t alpha)
{
    color.a = alpha;
    return color;
}
