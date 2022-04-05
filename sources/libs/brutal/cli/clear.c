#include <brutal/cli/clear.h>
#include <brutal/io/funcs.h>

void cli_clear_display(IoWriter writer)
{
    io_fmt$(writer, "\x1b[2J");
}

void cli_clear_display_after_cursor(IoWriter writer)
{
    io_fmt$(writer, "\x1b[J");
}

void cli_clear_display_before_cursor(IoWriter writer)
{
    io_fmt$(writer, "\x1b[1J");
}

void cli_clear_line(IoWriter writer)
{
    io_fmt$(writer, "\x1b[2K");
}

void cli_clear_line_after_cursor(IoWriter writer)
{
    io_fmt$(writer, "\x1b[K");
}

void cli_clear_line_before_cursor(IoWriter writer)
{
    io_fmt$(writer, "\x1b[1K");
}
