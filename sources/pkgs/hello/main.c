#include <brutal/debug.h>
#include <protos/test/hello.h>

static HelloError handle_hello_hello(IpcEv *ev, BrTask task, Str const *req, Str *resp, Alloc *alloc)
{
    UNUSED(ev);
    UNUSED(alloc);

    log$("task n°{} send us '{}'", task, *req);
    *resp = str$("world");
    return HELLO_SUCCESS;
}

static HelloVTable _hello_vtable = {
    handle_hello_hello,
};

int main()
{
    IpcEv ev = {};
    br_ev_init(&ev, nullptr, alloc_global());

    hello_impl(&ev, &_hello_vtable);

    return br_ev_run(&ev);
}
