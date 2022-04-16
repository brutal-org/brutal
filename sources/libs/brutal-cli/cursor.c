#include <brutal-fmt>

#include "cursor.h"

void cli_cursor_up(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}A", count);
}

void cli_cursor_down(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}B", count);
}

void cli_cursor_forward(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}C", count);
}

void cli_cursor_backward(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}D", count);
}

void cli_cursor_nextline(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}E", count);
}

void cli_cursor_prevline(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}F", count);
}

void cli_cursor_horizontal(IoWriter writer, int count)
{
    fmt_format$(writer, "\x1b[{}G", count);
}

void cli_cursor_position(IoWriter writer, int row, int col)
{
    fmt_format$(writer, "\x1b[{};{}H", row, col);
}

void cli_cursor_hide(IoWriter writer)
{
    fmt_format$(writer, "\x1b[?25l");
}

void cli_cursor_show(IoWriter writer)
{
    fmt_format$(writer, "\x1b[?25h");
}
