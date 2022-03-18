#pragma once

#include <brutal/io/buf.h>
#include <brutal/io/print.h>

typedef struct
{
    int indent;
    int indent_size;
    IoWriter writer;
    bool line_begin;
} Emit;

void emit_init(Emit *self, IoWriter writer);

void emit_deinit(Emit *self);

void emit_ident_size(Emit *self, int size);

void emit_ident(Emit *self);

void emit_deident(Emit *self);

IoWriter emit_writer(Emit *self);

#define emit_fmt$(SELF, FMT, ...) print_impl(emit_writer(SELF), str$(FMT), PRINT_ARGS(__VA_ARGS__));
