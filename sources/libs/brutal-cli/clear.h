#pragma once

#include <brutal-io>

void cli_clear_display(IoWriter writer);

void cli_clear_display_after_cursor(IoWriter writer);

void cli_clear_display_before_cursor(IoWriter writer);

void cli_clear_line(IoWriter writer);

void cli_clear_line_after_cursor(IoWriter writer);

void cli_clear_line_before_cursor(IoWriter writer);
