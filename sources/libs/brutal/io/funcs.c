#include <brutal/base/attributes.h>
#include <brutal/io/funcs.h>
#include <brutal/math/clamp.h>

/* --- Read Functions ------------------------------------------------------- */

static IoResult io_repeat_read(void *context, uint8_t *buf, size_t size)
{
    mem_set(buf, (uintptr_t)context, size);
    return OK(IoResult, size);
}

IoReader io_repeat(uint8_t byte)
{
    return (IoReader){
        // XXX: Find a better way to do this
        .context = (void *)(uintptr_t)byte,
        .read = io_repeat_read,
    };
}

static IoResult io_empty_read(MAYBE_UNUSED void *context, MAYBE_UNUSED uint8_t *buf, MAYBE_UNUSED size_t size)
{
    return OK(IoResult, 0);
}

IoReader io_empty(void)
{
    return (IoReader){
        .read = io_empty_read,
    };
}

static IoResult io_zero_read(MAYBE_UNUSED void *context, uint8_t *buf, size_t size)
{
    mem_set(buf, 0, size);
    return OK(IoResult, size);
}

IoReader io_zero(void)
{
    return (IoReader){
        .read = io_zero_read,
    };
}

IoResult io_read(IoReader self, uint8_t *data, size_t size)
{
    return self.read(self.context, data, size);
}

IoResult io_pread(IoRSeek self, uint8_t *data, size_t size, size_t offset)
{
    io_seek$(self, io_seek_from_start(offset));
    return io_read$(self, data, size);
}

IoResult io_read_line(IoReader self, Buf *buf, Alloc *alloc)
{
    buf_init(buf, 512, alloc);

    uint8_t c = 0;
    size_t read = TRY(IoResult, io_read_byte(self, &c));
    while (c != '\n')
    {
        TRY(IoResult, io_write_byte(buf_writer(buf), c));
        read = TRY(IoResult, io_read_byte(self, &c));
    }

    return OK(IoResult, read);
}

IoResult io_read_all(IoReader self, Buf *buf, Alloc *alloc)
{
    buf_init(buf, 512, alloc);
    return io_copy(self, buf_writer(buf));
}

IoResult io_read_byte(IoReader self, uint8_t *c)
{
    return io_read(self, c, 1);
}

/* --- Write Functions ------------------------------------------------------ */

static IoResult io_sink_write(MAYBE_UNUSED void *context, MAYBE_UNUSED uint8_t const *data, size_t size)
{
    return OK(IoResult, size);
}

IoWriter io_sink(void)
{
    return (IoWriter){
        .write = io_sink_write,
    };
}

IoResult io_write(IoWriter self, uint8_t const *data, size_t size)
{
    return self.write(self.context, data, size);
}

IoResult io_pwrite(IoWSeek self, uint8_t const *data, size_t size, size_t offset)
{
    io_seek$(self, io_seek_from_start(offset));
    return io_write$(self, data, size);
}

IoResult io_write_str(IoWriter self, Str str)
{
    return io_write(self, (uint8_t *)str.buf, str.len);
}

IoResult io_write_byte(IoWriter self, uint8_t c)
{
    return io_write(self, &c, 1);
}

IoResult io_fmt(IoWriter writer, Str format, AnyVa args)
{
    size_t current = 0;
    size_t written = 0;
    bool skip_fmt = false;
    Scan scan;
    scan_init(&scan, format);

    while (!scan_ended(&scan))
    {
        if (scan_skip_word(&scan, str$("{{")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, '{'));
        }
        else if (scan_skip_word(&scan, str$("}}")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, '}'));
        }
        else if (scan_curr(&scan) == '{' && !skip_fmt)
        {
            Fmt fmt = fmt_parse(&scan);

            if (current < args.len)
            {
                written += TRY(IoResult, fmt_any(fmt, writer, args.buf[current]));
            }
            else
            {
                written += TRY(IoResult, io_write_str(writer, str$("{}")));
            }

            current++;
        }
        else if (scan_curr(&scan) == '\\' && skip_fmt == false)
        {
            skip_fmt = true;
            scan_next(&scan);
        }
        else
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, scan_next(&scan)));
        }
    }

    return OK(IoResult, written);
}

IoResult io_vprintf(IoWriter writer, Str format, va_list va)
{
    Scan scan;
    size_t written = 0;
    scan_init(&scan, format);

    while (!scan_ended(&scan))
    {
        if (scan_skip_word(&scan, str$("%%")))
        {
            written += TRY(IoResult, io_write_byte(writer, '%'));
        }
        else if (scan_curr(&scan) == '%')
        {
            Fmt fmt = {};
            FmtPrintfType type = fmt_parse_printf(&scan, &fmt);

            switch (type)
            {
            default:
            case FMT_PRINTF_INT:
                written += TRY(IoResult, fmt_signed(fmt, writer, va_arg(va, int)));
                break;

            case FMT_PRINTF_LONG:
                written += TRY(IoResult, fmt_signed(fmt, writer, va_arg(va, long)));
                break;

            case FMT_PRINTF_LONGLONG:
                written += TRY(IoResult, fmt_signed(fmt, writer, va_arg(va, long long)));
                break;

            case FMT_PRINTF_UINT:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, unsigned int)));
                break;

            case FMT_PRINTF_ULONG:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, unsigned long)));
                break;

            case FMT_PRINTF_ULONGLONG:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, unsigned long long)));
                break;

#ifndef __freestanding__
            case FMT_PRINTF_FLOAT:
                written += TRY(IoResult, fmt_float(fmt, writer, va_arg(va, double)));
                break;
#endif

            case FMT_PRINTF_STRING:
                written += TRY(IoResult, fmt_string(fmt, writer, str$(va_arg(va, char const *))));
                break;

            case FMT_PRINTF_PTR:
                written += TRY(IoResult, fmt_unsigned(fmt, writer, va_arg(va, uintptr_t)));
                break;
            }
        }
        else
        {
            written += TRY(IoResult, io_write_byte(writer, scan_next(&scan)));
        }
    }

    return OK(IoResult, written);
}

IoResult io_printf(IoWriter writer, Str format, ...)
{
    va_list va;
    va_start(va, format);
    IoResult result = io_vprintf(writer, format, va);
    va_end(va);
    return result;
}

/* --- Seek Functions ------------------------------------------------------- */

IoResult io_seek(IoSeeker self, IoSeek off)
{
    return self.seek(self.context, off);
}

IoResult io_tell(IoSeeker self)
{
    return io_seek(self, io_seek_from_current(0));
}

IoResult io_size(IoSeeker self)
{
    size_t current = TRY(IoResult, io_tell(self));
    size_t result = TRY(IoResult, io_seek(self, io_seek_from_end(0)));
    TRY(IoResult, io_seek(self, io_seek_from_start(current)));
    return OK(IoResult, result);
}

IoResult io_skip(IoReader self, size_t size)
{
    return io_copy_n(self, io_sink(), size);
}

/* --- Copy Functions ------------------------------------------------------- */

IoResult io_copy(IoReader from, IoWriter to)
{
    size_t total = 0;

    do
    {
        uint8_t buf[512];
        size_t read = TRY(IoResult, io_read(from, buf, 512));

        if (read == 0)
        {
            return OK(IoResult, total);
        }

        size_t written = TRY(IoResult, io_write(to, buf, read));

        if (written == 0)
        {
            return OK(IoResult, total);
        }

        total += written;
    } while (1);
}

IoResult io_copy_n(IoReader from, IoWriter to, size_t size)
{
    size_t total = 0;

    do
    {
        uint8_t buf[512];
        size_t to_read = m_min(512, size - total);
        size_t read = TRY(IoResult, io_read(from, buf, to_read));

        if (read == 0)
        {
            return OK(IoResult, total);
        }

        size_t written = TRY(IoResult, io_write(to, buf, read));

        if (written == 0)
        {
            return OK(IoResult, total);
        }

        total += written;
    } while (total < size);

    return OK(IoResult, total);
}
