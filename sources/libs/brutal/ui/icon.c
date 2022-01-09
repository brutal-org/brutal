#include <brutal/alloc/global.h>
#include <brutal/ui/icon.h>

void ui_icon_paint(MAYBE_UNUSED UiView *self, Gfx *gfx)
{
    gfx_fill(gfx, gfx_paint_fill(GFX_CYAN));
    gfx_rect(gfx, ui_view_container(self), 0);

    gfx_fill(gfx, gfx_paint_fill(GFX_BLACK));
    gfx_path(gfx, str$("M 120 80 L 106.7 80.9 C 98.1 70.7 74 45 50 45 C 50 45 30.3 74.6 49.6 114.1 C 44.1 122.4 40.7 126.7 40 136.6 L 20.7 139.5 L 22.8 149.3 L 40.4 146.7 L 41.8 153.8 L 26.1 163.2 L 30.8 172.1 L 45.3 163.2 C 56.8 187.6 85.9 200 120 200 C 154.1 200 183.2 187.6 194.7 163.2 L 209.2 172.1 L 213.9 163.2 L 198.2 153.8 L 199.6 146.7 L 217.2 149.3 L 219.3 139.5 L 200 136.6 C 199.3 126.7 195.9 122.4 190.4 114.1 C 209.7 74.6 190 45 190 45 C 166 45 141.9 70.7 133.3 80.9 L 120 80 M 90 110 A 10 10 90 0 1 100 120 A 10 10 90 0 1 90 130 A 10 10 90 0 1 80 120 A 10 10 90 0 1 90 110 M 150 110 A 10 10 90 0 1 160 120 A 10 10 90 0 1 150 130 A 10 10 90 0 1 140 120 A 10 10 90 0 1 150 110 M 110 140 H 130 L 123 153.9 C 125 160.3 130.6 165 137.5 165 A 15 15 90 0 0 152.5 150 H 157.5 A 20 20 90 0 1 137.5 170 C 130 170 123.5 165.9 120 160 V 160 H 120 C 116.5 165.9 110 170 102.5 170 A 20 20 90 0 1 82.5 150 H 87.5 A 15 15 90 0 0 102.5 165 C 109.4 165 115 160.3 117 153.9 L 110 140 Z"));
}

UiView *ui_icon_create(void)
{
    UiView *self = ui_view_create(UiIcon);

    self->paint = ui_icon_paint;

    return self;
}
