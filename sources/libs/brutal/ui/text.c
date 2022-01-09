#include <brutal/alloc/global.h>
#include <brutal/font/font.h>
#include <brutal/ui/text.h>

float font_scale(UiTextStyle style)
{
    if (style == UI_TEXT_HEADLINE)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void ui_text_paint(UiView *self, Gfx *gfx)
{
    float scale = font_scale(ui_text$(self)->style);

    BFontMesure mesures = bfont_mesure(bfont_builtin(), ui_text$(self)->text, scale);
    MRect centered = m_gravity_apply(self->style.gravity, M_FLOW_LEFT_TO_RIGHT, mesures.capbound, ui_view_content(self));
    MVec2 orgin = m_vec2_add(centered.pos, mesures.baseline);

    gfx_color(gfx, ui_text$(self)->color);
    gfx_text(gfx, orgin, ui_text$(self)->text, scale);
}

MRect ui_text_size(UiView *self)
{
    float scale = font_scale(ui_text$(self)->style);
    return bfont_mesure(bfont_builtin(), ui_text$(self)->text, scale).linebound;
}

UiView *ui_text_create(Str text, UiTextStyle style, GfxColor color)
{
    UiView *self = ui_view_create(UiText);

    self->paint = ui_text_paint;
    self->size = ui_text_size;

    ui_text$(self)->text = text;
    ui_text$(self)->color = color;
    ui_text$(self)->style = style;

    return self;
}
