#include <brutal/alloc.h>
#include <brutal/codec/tga.h>
#include <brutal/gfx.h>
#include <brutal/io.h>
#include "test/test.h"

/*
TEST(gfx_draw_rectangle)
{
    GfxCtx canvas;

    gfx_ctx_init(&canvas, test_alloc(), 512, 512, GFX_PIXEL_FORMAT_RGBA8888);
    gfx_ctx_begin(&canvas);

    gfx_ctx_fill(&canvas, gfx_paint_fill(GFX_COLOR_BLACK));
    gfx_ctx_clear(&canvas);

    gfx_ctx_fill(&canvas, gfx_paint_fill(GFX_COLOR_RED));
    gfx_ctx_rect(&canvas, (Rectf){64, 64, 384, 384});

    gfx_ctx_end(&canvas);

    IoFile file;
    io_file_create(&file, str$("test.tga"));
    IoWriter writer = io_file_writer(&file);
    tga_encode(&writer, gfx_ctx_surface(&canvas));
    io_file_close(&file);

    gfx_ctx_deinit(&canvas);
}
*/
