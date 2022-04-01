#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct ImageElement : public Element
{
private:
    Ref<Graphic::Bitmap> _bitmap;
    Graphic::BitmapScaling _scaling = Graphic::BitmapScaling::FIT;

public:
    ImageElement(Ref<Graphic::Bitmap> bitmap, Graphic::BitmapScaling scaling = Graphic::BitmapScaling::FIT);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    virtual Math::Vec2i size() override;
};

WIDGET_BUILDER(ImageElement, image);

} // namespace Widget
