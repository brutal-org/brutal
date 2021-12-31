#include <brutal/alloc/global.h>
#include <brutal/font/font.h>
#include <brutal/ui/text.h>

void ui_text_paint(UiView *self, Gfx *gfx)
{
    BFontMesure mesures = bfont_mesure(bfont_builtin(), ui_text$(self)->text);
    MRect centered = m_gravity_apply(M_GRAVITY_CENTER, M_FLOW_LEFT_TO_RIGHT, mesures.bound, ui_view_content(self));
    MVec2 orgin = m_vec2_add(centered.pos, mesures.baseline);

    gfx_color(gfx, GFX_BLACK);
    gfx_text(gfx, orgin, ui_text$(self)->text);
}

MRect ui_text_size(UiView *self)
{
    return bfont_mesure(bfont_builtin(), ui_text$(self)->text).bound;
}

UiView *ui_text_create(Str text)
{
    UiView *self = ui_view_create(UiText);

    self->paint = ui_text_paint;
    self->size = ui_text_size;

    ui_text$(self)->text = text;

    return self;
}
