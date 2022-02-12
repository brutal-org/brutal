#include <brutal/alloc/global.h>
#include <brutal/ui/app.h>
#include <brutal/ui/text.h>

GfxFontMesure ui_text_mesure(UiView *self)
{
    return gfx_font_mesure(ui_app_self()->font, ui_text$(self)->font, ui_text$(self)->text);
}

void ui_text_repaint(UiView *self, Gfx *gfx)
{
    GfxFontMesure mesures = ui_text_mesure(self);
    MRect centered = m_gravity_apply(self->layout.gravity, M_FLOW_LEFT_TO_RIGHT, mesures.capbound, ui_view_content(self));
    MVec2 orgin = m_vec2_add(centered.pos, mesures.baseline);

    gfx_color(gfx, ui_view_color(self, UI_COLOR_ROLE_TEXT));
    gfx_font_style(gfx, ui_text$(self)->font);
    gfx_text(gfx, orgin, ui_text$(self)->text);
}

MRect ui_text_size(UiView *self)
{
    return ui_text_mesure(self).linebound;
}

UiView *ui_text_create(Str text, GfxFontStyle font)
{
    UiView *self = ui_view_create(UiText);

    self->repaint = ui_text_repaint;
    self->size = ui_text_size;

    ui_text$(self)->text = text;
    ui_text$(self)->font = font;

    return self;
}
