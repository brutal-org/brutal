#pragma once

#include <libgraphic/Bitmap.h>
#include <libutils/String.h>

namespace Graphic
{

#define ICON_SIZE_LIST(__ENTRY) \
    __ENTRY(18)                 \
    __ENTRY(24)                 \
    __ENTRY(36)                 \
    __ENTRY(48)

#define ICON_SIZE_ENUM_ENTRY(__size) ICON_##__size##PX,
enum IconSize
{
    ICON_SIZE_LIST(ICON_SIZE_ENUM_ENTRY) __ICON_SIZE_COUNT,
};

struct Icon : public Shared<Icon>
{
private:
    String _name;
    Ref<Bitmap> _bitmaps[__ICON_SIZE_COUNT] = {};

public:
    static Ref<Icon> get(String name);

    String &name() { return _name; }

    Icon(String name);

    ~Icon();

    Math::Recti bound(IconSize size);

    Ref<Bitmap> bitmap(IconSize size);

    void set_bitmap(IconSize size, Ref<Bitmap> bitmap);
};

} // namespace Graphic
