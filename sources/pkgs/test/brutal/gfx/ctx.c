#include <brutal/alloc.h>
#include <brutal/codec/tga/tga.h>
#include <brutal/io.h>
#include "test/test.h"

/*
TEST(gfx_draw_rectangle)
{
    Gfx canvas;

    gfx_init(&canvas, test_alloc(), 512, 512, GFX_FMT_RGBA8888);
    gfx_begin(&canvas);

    gfx_fill(&canvas, gfx_paint_fill(GFX_BLACK));
    gfx_clear(&canvas);

    gfx_fill(&canvas, gfx_paint_fill(GFX_RED));
    gfx_fill_rect(&canvas, m_rect(64, 64, 384, 384), 0);

    gfx_end(&canvas);

    IoFile file;
    io_file_create(&file, str$("test.tga"));
    IoWriter writer = io_file_writer(&file);
    tga_encode(&writer, gfx_surface(&canvas));
    io_file_close(&file);

    gfx_deinit(&canvas);
}
*/
