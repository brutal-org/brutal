#include <brutal-debug>
#include <brutal-fmt>

#include "emit.h"
#include "funcs.h"

void emit_init(Emit *self, IoWriter writer)
{
    self->indent = 0;
    self->indent_size = 4;
    self->writer = writer;
    self->line_begin = true;
}

void emit_deinit(Emit *self)
{
    *self = (Emit){};
}

void emit_ident_size(Emit *self, int size)
{
    self->indent_size = size;
}

void emit_ident(Emit *self)
{
    self->indent++;
}

void emit_deident(Emit *self)
{
    assert_greater_equal(self->indent, 0);
    self->indent--;
}

static IoResult emit_write_impl(void *ctx, uint8_t const *data, size_t size)
{
    Emit *self = (Emit *)ctx;
    size_t written = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (self->line_begin)
        {
            for (int j = 0; j < self->indent_size * self->indent; j++)
            {
                written += TRY(IoResult, io_write_byte(self->writer, ' '));
            }

            self->line_begin = false;
        }

        if (data[i] == '\n')
        {
            self->line_begin = true;
        }

        written += TRY(IoResult, io_write_byte(self->writer, data[i]));
    }

    return OK(IoResult, written);
}

IoWriter emit_writer(Emit *self)
{
    return (IoWriter){
        .write = emit_write_impl,
        .context = self,
    };
}

IoResult emit_format(Emit *self, Str fmt, AnyVa args)
{
    return fmt_format(emit_writer(self), fmt, args);
}
