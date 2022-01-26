#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/gfx/font.h>
#include <brutal/io.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("Usage: fontinfo [OPTION]... [INPUT]", argv[0]);
        return -1;
    }

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, str$(argv[1])), "File not found!");

    GfxFont font = gfx_font_ssfn2(io_file_rseek(&source_file));
    log$("Style: ", font.style.width);
    return 0;
}
