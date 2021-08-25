#include <brutal/io/emit.h>
#include <brutal/log/assert.h>

void emit_init(Emit *self, IoWriter *writer)
{
    self->indent = 0;
    self->writer = writer;
    self->line_begin = true;
}

void emit_deinit(Emit *self)
{
    *self = (Emit){};
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

static IoWriteResult emit_write_impl(Emit *self, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    size_t written = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (self->line_begin)
        {
            for (int j = 0; j < 4 * self->indent; j++)
            {
                written += TRY(IoWriteResult, io_put(self->writer, ' '));
            }

            self->line_begin = false;
        }

        if (data[i] == '\n')
        {
            self->line_begin = true;
        }

        written += TRY(IoWriteResult, io_put(self->writer, data[i]));
    }

    return OK(IoWriteResult, written);
}

IoWriter emit_writer(Emit *self)
{
    return (IoWriter){
        .write = (IoWrite *)emit_write_impl,
        .context = self,
    };
}
