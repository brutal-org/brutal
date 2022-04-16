#pragma once

#include <brutal-io>
#include <brutal-parse>

typedef enum
{
    CLI_BLACK,
    CLI_RED,
    CLI_GREEN,
    CLI_YELLOW,
    CLI_BLUE,
    CLI_MAGENTA,
    CLI_CYAN,
    CLI_WHITE,
} CliIndex;

typedef struct
{
    uint8_t r, g, b;
} CliRgb;

typedef struct _Color
{
    enum
    {
        CLI_NONE,
        CLI_INDEX,
        CLI_BRIGHT,
        CLI_RGB
    } type;

    union
    {
        CliIndex index;
        CliRgb rgb;
    };
} CliColor;

#define CLI_STYLE_BOLD (1 << 0)
#define CLI_STYLE_DIM (1 << 1)
#define CLI_STYLE_ITALIC (1 << 2)
#define CLI_STYLE_UNDERLINE (1 << 3)
#define CLI_STYLE_BLINK (1 << 4)
#define CLI_STYLE_REVERSE (1 << 5)

typedef uint64_t CliFlags;

typedef struct
{
    CliFlags flags;
    CliColor foreground;
    CliColor background;
} CliStyle;

CliStyle cli_style(char const *style);

bool cli_style_parse(Scan *scan, CliStyle *style);

void cli_style_reset(IoWriter writer);

void cli_style_apply(IoWriter writer, CliStyle style);
