#include "color.h"

void ui_palette_init(UiPalette *self)
{
    ui_palette_overide(self, UI_COLOR_ROLE_TEXT, UI_COLOR_BASE09);
    ui_palette_overide(self, UI_COLOR_ROLE_BODY, UI_COLOR_BASE00);
    ui_palette_overide(self, UI_COLOR_ROLE_ACCENT, UI_COLOR_BLUE);
    ui_palette_overide(self, UI_COLOR_ROLE_BORDER, UI_COLOR_BASE02);
    ui_palette_overide(self, UI_COLOR_ROLE_PANEL, UI_COLOR_BASE01);
}

bool ui_palette_color(UiPalette *self, UiRole role, GfxColor *color)
{
    if (self->colors[role].present)
    {
        *color = self->colors[role].color;
        return true;
    }

    return false;
}

void ui_palette_overide(UiPalette *self, UiRole role, GfxColor color)
{
    self->colors[role].present = true;
    self->colors[role].color = color;
}
