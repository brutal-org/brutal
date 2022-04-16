#pragma once

#include <brutal-io>

void cli_cursor_up(IoWriter writer, int count);

void cli_cursor_down(IoWriter writer, int count);

void cli_cursor_forward(IoWriter writer, int count);

void cli_cursor_backward(IoWriter writer, int count);

void cli_cursor_nextline(IoWriter writer, int count);

void cli_cursor_prevline(IoWriter writer, int count);

void cli_cursor_horizontal(IoWriter writer, int count);

void cli_cursor_position(IoWriter writer, int row, int col);

void cli_cursor_hide(IoWriter writer);

void cli_cursor_show(IoWriter writer);
