#include <brutal-fmt>
#include <brutal-parse>

#include "fmt.h"
#include "style.h"

IoResult cli_fmt(IoWriter writer, Str format, AnyVa args)
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
        if (scan_skip_word(&scan, str$("[[")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, '{'));
        }
        else if (scan_skip_word(&scan, str$("]]")))
        {
            skip_fmt = false;
            written += TRY(IoResult, io_write_byte(writer, '}'));
        }
        else if (scan_skip(&scan, '[') && !skip_fmt)
        {
            scan_eat(&scan, isspace);

            CliStyle style = {};
            while (scan_curr(&scan) != ']' &&
                   !scan_ended(&scan))
            {
                cli_style_parse(&scan, &style);
                scan_eat(&scan, isspace);
            }

            scan_expect(&scan, ']');

            if (scan_expect(&scan, '{'))
            {
                Fmt fmt = fmt_parse(&scan);

                if (current < args.len)
                {
                    cli_style_apply(writer, style);
                    written += TRY(IoResult, fmt_any(fmt, writer, args.buf[current]));
                    cli_style_reset(writer);
                }
                else
                {
                    written += TRY(IoResult, io_write_str(writer, str$("{}")));
                }
            }

            current++;
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
