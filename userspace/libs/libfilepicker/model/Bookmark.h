#pragma once

#include <libgraphic/Icon.h>
#include <libio/Path.h>
#include <libjson/Json.h>
#include <libutils/Ref.h>

namespace FilePicker
{

struct Bookmark
{
private:
    String _name;
    Ref<Graphic::Icon> _icon;
    IO::Path _path;

public:
    const String &name() const
    {
        return _name;
    }

    const Ref<Graphic::Icon> icon() const
    {
        return _icon;
    }

    const IO::Path &path() const
    {
        return _path;
    }

    Bookmark(const Json::Value &value)
    {
        if (value.is(Json::OBJECT))
        {
            value.with("name", [&](auto &value)
                { _name = value.as_string(); });

            value.with("icon", [&](auto &value)
                { _icon = Graphic::Icon::get(value.as_string()); });

            value.with("path", [&](auto &value)
                { _path = IO::Path::parse(value.as_string()); });
        }
    }

    Bookmark(const String &name, const Ref<Graphic::Icon> icon, const IO::Path &path)
        : _name(name),
          _icon(icon),
          _path(path)
    {
    }

    bool operator==(const Bookmark &other) const
    {
        return _path == other._path;
    }

    Json::Value serialize()
    {
        Json::Value::Object obj{};

        obj["name"] = _name;
        obj["icon"] = _icon->name();
        obj["path"] = _path.string();

        return obj;
    }
};

} // namespace FilePicker