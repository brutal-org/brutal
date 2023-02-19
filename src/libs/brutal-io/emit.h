#pragma once

#include "buf.h"

typedef struct
{
    int indent;
    int indent_size;
    bool line_begin;

    IoWriter writer;
} Emit;

void emit_init(Emit *self, IoWriter writer);

void emit_deinit(Emit *self);

void emit_ident_size(Emit *self, int size);

void emit_ident(Emit *self);

void emit_deident(Emit *self);

IoWriter emit_writer(Emit *self);

IoResult emit_format(Emit *self, Str fmt, AnyVa args);

#define emit_fmt$(SELF, FMT, ...) \
    emit_format(SELF, str$(FMT), any_va$(__VA_ARGS__))
