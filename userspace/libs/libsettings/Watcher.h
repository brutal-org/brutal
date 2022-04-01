#pragma once

#include <libutils/Func.h>

#include <libsettings/Path.h>
#include <libsettings/Service.h>

namespace Settings
{

using WatcherCallback = Func<void(const Json::Value &)>;

struct Watcher
{
private:
    Path _path;
    WatcherCallback _callback;
    Ref<Service> _service;

public:
    const Path &path() const
    {
        return _path;
    }

    Watcher(Path path, WatcherCallback callback)
        : _path{path}, _callback{callback}, _service{Service::the()}
    {
        _service->register_watcher(*this);
    }

    ~Watcher()
    {
        _service->unregister_watcher(*this);
    }

    void invoke(const Json::Value &value)
    {
        _callback(value);
    }
};

} // namespace Settings
