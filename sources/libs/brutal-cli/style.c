#include <brutal-fmt>

#include "style.h"

CliStyle cli_style(char const *str)
{
    Scan scan;
    CliStyle style = {};
    scan_init(&scan, str$(str));
    cli_style_parse(&scan, &style);
    return style;
}

bool cli_pase_color_name(Scan *scan, CliIndex *index)
{
    if (scan_skip_word(scan, str$("black")))
    {
        *index = CLI_BLACK;
    }
    else if (scan_skip_word(scan, str$("red")))
    {
        *index = CLI_RED;
    }
    else if (scan_skip_word(scan, str$("green")))
    {
        *index = CLI_GREEN;
    }
    else if (scan_skip_word(scan, str$("yellow")))
    {
        *index = CLI_YELLOW;
    }
    else if (scan_skip_word(scan, str$("blue")))
    {
        *index = CLI_BLUE;
    }
    else if (scan_skip_word(scan, str$("magenta")))
    {
        *index = CLI_MAGENTA;
    }
    else if (scan_skip_word(scan, str$("cyan")))
    {
        *index = CLI_CYAN;
    }
    else if (scan_skip_word(scan, str$("white")))
    {
        *index = CLI_WHITE;
    }
    else
    {
        return false;
    }

    return true;
}

void cli_parse_color(Scan *scan, CliColor *color)
{
    if (scan_skip_word(scan, str$("light-")))
    {
        color->type = CLI_BRIGHT;
        cli_pase_color_name(scan, &color->index);
    }
    else if (cli_pase_color_name(scan, &color->index))
    {
        color->type = CLI_INDEX;
    }
    else
    {
        // TODO: rgba color support
    }
}

bool cli_style_parse(Scan *scan, CliStyle *style)
{
    if (scan_skip_word(scan, str$("bold")))
    {
        style->flags |= CLI_STYLE_BOLD;
    }
    else if (scan_skip_word(scan, str$("bold")))
    {
        style->flags |= CLI_STYLE_DIM;
    }
    else if (scan_skip_word(scan, str$("italic")))
    {
        style->flags |= CLI_STYLE_ITALIC;
    }
    else if (scan_skip_word(scan, str$("underline")))
    {
        style->flags |= CLI_STYLE_UNDERLINE;
    }
    else if (scan_skip_word(scan, str$("blink")))
    {
        style->flags |= CLI_STYLE_BLINK;
    }
    else if (scan_skip_word(scan, str$("reverse")))
    {
        style->flags |= CLI_STYLE_REVERSE;
    }
    else if (scan_skip_word(scan, str$("fg-")))
    {
        cli_parse_color(scan, &style->foreground);
    }
    else if (scan_skip_word(scan, str$("bg-")))
    {
        cli_parse_color(scan, &style->background);
    }
    else
    {
        return false;
    }

    return true;
}

void cli_style_reset(IoWriter writer)
{
    fmt_format$(writer, "\x1b[0m");
}

void cli_style_apply(IoWriter writer, CliStyle style)
{
    fmt_format$(writer, "\x1b[");

    bool need_sep = false;

    if (style.flags & CLI_STYLE_BOLD)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        io_write_byte(writer, '1');

        need_sep = true;
    }

    if (style.flags & CLI_STYLE_DIM)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        io_write_byte(writer, '2');

        need_sep = true;
    }

    if (style.flags & CLI_STYLE_ITALIC)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        io_write_byte(writer, '3');

        need_sep = true;
    }

    if (style.flags & CLI_STYLE_UNDERLINE)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        io_write_byte(writer, '4');

        need_sep = true;
    }

    if (style.flags & CLI_STYLE_BLINK)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        io_write_byte(writer, '5');

        need_sep = true;
    }

    if (style.flags & CLI_STYLE_REVERSE)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        io_write_byte(writer, '7');

        need_sep = true;
    }

    if (style.foreground.type == CLI_RGB)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        CliRgb rgb = style.foreground.rgb;
        fmt_format$(writer, "38;2;{};{};{}", rgb.r, rgb.g, rgb.b);

        need_sep = true;
    }
    else if (style.foreground.type == CLI_INDEX)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        int color = style.foreground.index + 30;
        fmt_format$(writer, "{}", color);

        need_sep = true;
    }
    else if (style.foreground.type == CLI_BRIGHT)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        int color = style.foreground.index + 90;
        fmt_format$(writer, "{}", color);

        need_sep = true;
    }

    if (style.background.type == CLI_RGB)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        CliRgb rgb = style.background.rgb;
        fmt_format$(writer, "48;2;{};{};{}", rgb.r, rgb.g, rgb.b);

        need_sep = true;
    }
    else if (style.background.type == CLI_INDEX)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        int color = style.background.index + 40;
        fmt_format$(writer, "{}", color);

        need_sep = true;
    }
    else if (style.background.type == CLI_BRIGHT)
    {
        if (need_sep)
            io_write_byte(writer, ';');

        int color = style.background.index + 100;
        fmt_format$(writer, "{}", color);

        need_sep = true;
    }

    fmt_format$(writer, "m");
}
