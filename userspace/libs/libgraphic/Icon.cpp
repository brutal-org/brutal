#include <stdio.h>
#include <string.h>

#include <libgraphic/Icon.h>
#include <libio/Format.h>
#include <libio/Path.h>
#include <libutils/HashMap.h>

namespace Graphic
{

static HashMap<String, Ref<Icon>>
    _icons{};

#define ICON_SIZES_ENTRY(__size) __size,
const int _icon_sizes[] = {ICON_SIZE_LIST(ICON_SIZES_ENTRY)};

static Ref<Icon> icon_load(String name)
{
    auto icon = make<Icon>(name);

    for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
    {
        auto path = IO::format("/Files/Icons/{}.svg", name);
        auto bitmap_or_result = Bitmap::load_from(path, _icon_sizes[i]);

        if (bitmap_or_result.success())
        {
            icon->set_bitmap(static_cast<IconSize>(i), bitmap_or_result.unwrap());
        }
    }

    return icon;
}

Ref<Icon> Icon::get(String name)
{
    if (!_icons.has_key(name))
    {
        _icons[name] = icon_load(name);
    }

    return _icons[name];
}

Icon::Icon(String name)
    : _name(name)
{
}

Icon::~Icon()
{
}

Math::Recti Icon::bound(IconSize size)
{
    return bitmap(size)->bound();
}

Ref<Bitmap> Icon::bitmap(IconSize size)
{
    if (!_bitmaps[size])
    {
        for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
        {
            if (_bitmaps[i])
            {
                return _bitmaps[i];
            }
        }

        _bitmaps[size] = Bitmap::load_from_or_placeholder("none");
    }

    return _bitmaps[size];

    ASSERT_NOT_REACHED();
}

void Icon::set_bitmap(IconSize size, Ref<Bitmap> bitmap)
{
    _bitmaps[size] = bitmap;
}

} // namespace Graphic
