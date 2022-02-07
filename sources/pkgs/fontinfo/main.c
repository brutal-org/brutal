#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/gfx/ssfn2/ssfn2.h>
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

    SSFN2Font ssfn_font;
    if (!font_ssfn2_init(io_file_rseek(&source_file), &ssfn_font).succ)
    {
        return -1;
    }
    GfxFont font = gfx_font_ssfn2(&ssfn_font);
    log$("Style: {}", font.style.width);

    font_ssfn2_deinit(&ssfn_font);
    return 0;
}
