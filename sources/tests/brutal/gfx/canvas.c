#include <brutal/alloc.h>
#include <brutal/codec/tga.h>
#include <brutal/gfx.h>
#include <brutal/io.h>
#include "tests/test.h"

TEST(gfx_draw_rectangle)
{
    GfxCanvas canvas;

    gfx_canvas_init(&canvas, alloc_global(), 512, 512, GFX_PIXEL_FORMAT_RGBA8888);
    gfx_canvas_begin(&canvas);

    gfx_canvas_fill(&canvas, gfx_paint_fill(GFX_COLOR_BLACK));
    gfx_canvas_clear(&canvas);

    gfx_canvas_fill(&canvas, gfx_paint_fill(GFX_COLOR_RED));
    gfx_canvas_rect(&canvas, (Rectf){64, 64, 384, 384});

    gfx_canvas_end(&canvas);

    IoFile file;
    io_file_create(&file, str$("test.tga"));
    IoFileWriter writer = io_file_write(&file);
    tga_encode(base$(&writer), gfx_canvas_surface(&canvas));
    io_file_close(&file);

    gfx_canvas_deinit(&canvas);
}
