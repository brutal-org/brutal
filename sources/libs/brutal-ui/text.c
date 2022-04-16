#include "text.h"

#include "app.h"

GfxFontMeasure ui_text_measure(UiView *self)
{
    return gfx_font_measure(ui_app_self()->font, ui_text$(self)->font, ui_text$(self)->text);
}

void ui_text_repaint(UiView *self, Gfx *gfx)
{
    GfxFontMeasure measures = ui_text_measure(self);
    MRectf centered = m_gravity_apply(self->layout.gravity, M_FLOW_LEFT_TO_RIGHT, measures.capbound, ui_view_content(self));
    MVec2f orgin = m_vec2f_add(centered.pos, measures.baseline);

    gfx_fill_style(gfx, gfx_paint_fill(ui_view_color(self, UI_COLOR_ROLE_TEXT)));
    gfx_font_style(gfx, ui_text$(self)->font);
    gfx_text(gfx, orgin, ui_text$(self)->text);
}

MRectf ui_text_size(UiView *self)
{
    return ui_text_measure(self).linebound;
}

UiView *ui_text_create(Str text, GfxFontStyle font)
{
    UiView *self = ui_view_create$(UiText);

    self->repaint = ui_text_repaint;
    self->size = ui_text_size;

    ui_text$(self)->text = text;
    ui_text$(self)->font = font;

    return self;
}
