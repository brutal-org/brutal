#pragma once

#include <libasync/Source.h>
#include <libutils/Func.h>

namespace Async
{

struct Invoker : public Source
{
private:
    bool _invoke_later = false;
    Func<void()> _callback;
    Ref<Loop> _eventloop;

public:
    Invoker(Func<void()> callback) : _callback(callback)
    {
        loop().register_invoker(this);
    }

    ~Invoker()
    {
        loop().unregister_invoker(this);
    }

    [[nodiscard]] bool should_be_invoke_later()
    {
        return _invoke_later;
    }

    void invoke()
    {
        _invoke_later = false;
        _callback();
    }

    void invoke_later()
    {
        _invoke_later = true;
    }

    void cancel()
    {
        _invoke_later = false;
    }
};

} // namespace Async
