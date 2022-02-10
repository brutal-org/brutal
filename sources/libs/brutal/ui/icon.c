#include <brutal/alloc/global.h>
#include <brutal/alloc/heap.h>
#include <brutal/ui/icon.h>

void ui_icon_paint(MAYBE_UNUSED UiView *self, Gfx *gfx)
{
    gfx_fill_style(gfx, gfx_paint_fill(GFX_BLACK));
    gfx_fill_rect(gfx, ui_view_container(self), 0);

    gfx_fill_style(gfx, gfx_paint_fill(GFX_RED));
    gfx_fill_rect(gfx, m_rect(100, 100, 100, 100), 32);

    gfx_fill_style(gfx, gfx_paint_fill(GFX_WHITE));
    gfx_text(gfx, m_vec2(100, 100), str$("B"), gfx_font_ssfn2(&ui_icon$(self)->font));
}

UiView *ui_icon_create(void)
{
    UiView *self = ui_view_create(UiIcon);

    self->paint = ui_icon_paint;

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, str$("sysroot/pkgs/font-ssfn2/FreeSans.sfn.gz")), "File not found!");

    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    UNWRAP_OR_PANIC(ssfn2_load(io_file_rseek(&source_file), &ui_icon$(self)->font, base$(&heap)), "Failed to load font!");

    return self;
}
