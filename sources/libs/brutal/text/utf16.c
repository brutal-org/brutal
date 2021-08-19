#include <brutal/io/buffer.h>
#include <brutal/io/scan.h>
#include <brutal/text/case.h>

uint16_t *str_to_cstr_utf16(Str str, Alloc *alloc)
{
    size_t size = (str.len + 1) * 2;
    uint16_t *utf16 = (uint16_t *)alloc_malloc(alloc, size);

    for (size_t i = 0; i < str.len; i++)
    {
        utf16[i] = str.buffer[i];
    }

    utf16[str.len] = 0;

    return utf16;
}

uint16_t *str_to_cstr_utf16_dos(Str str, Alloc *alloc)
{
    Buffer buffer;
    buffer_init(&buffer, (str.len + 1) * 2, alloc);

    Scan scan;
    scan_init(&scan, str);

    while (!scan_end(&scan))
    {
        if (scan_skip_word(&scan, str$("\r\n")))
        {
            buffer_push(&buffer, u'\r');
            buffer_push(&buffer, u'\n');
        }
        else if (scan_skip(&scan, '\n'))
        {
            buffer_push(&buffer, u'\r');
            buffer_push(&buffer, u'\n');
        }
        else
        {
            buffer_push(&buffer, (uint16_t)scan_next(&scan));
        }
    }

    buffer_push(&buffer, (uint16_t)0x0);

    return (uint16_t *)buffer.data;
}
