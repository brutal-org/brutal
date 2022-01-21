#include <brutal/alloc/global.h>
#include <brutal/ui/text.h>

void ui_text_paint(UiView *self, Gfx *gfx)
{
    GfxFontMesure mesures = gfx_font_mesure(ui_text$(self)->font, ui_text$(self)->text);
    MRect centered = m_gravity_apply(self->style.gravity, M_FLOW_LEFT_TO_RIGHT, mesures.capbound, ui_view_content(self));
    MVec2 orgin = m_vec2_add(centered.pos, mesures.baseline);

    gfx_color(gfx, ui_text$(self)->color);
    gfx_fill_text(gfx, orgin, ui_text$(self)->text, ui_text$(self)->font);
}

MRect ui_text_size(UiView *self)
{
    return gfx_font_mesure(ui_text$(self)->font, ui_text$(self)->text).linebound;
}

UiView *ui_text_create(Str text, GfxFont font, GfxColor color)
{
    UiView *self = ui_view_create(UiText);

    self->paint = ui_text_paint;
    self->size = ui_text_size;

    ui_text$(self)->text = text;
    ui_text$(self)->font = font;
    ui_text$(self)->color = color;

    return self;
}
