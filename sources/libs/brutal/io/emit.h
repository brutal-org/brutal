#pragma once

#include <brutal/io/buffer.h>

typedef struct
{
    int indent;
    IoWriter *writer;
    bool line_begin;
} Emit;

void emit_init(Emit *self, IoWriter *writer);

void emit_deinit(Emit *self);

void emit_ident(Emit *self);

void emit_deident(Emit *self);

IoWriter emit_writer(Emit *self);

#define emit_fmt(EMIT, FMT, ...) (                                 \
    {                                                              \
        IoWriter __writer = emit_writer((EMIT));                   \
        print_impl(&__writer, str$(FMT), PRINT_ARGS(__VA_ARGS__)); \
    })
