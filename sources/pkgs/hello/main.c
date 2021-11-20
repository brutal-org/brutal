#include <brutal/debug.h>
#include <protos/test/hello.h>

// FIXME: include generated file into the build
#include <protos/test/hello.c>

static HelloError hello_hello_handler(IpcEv *ev, BrTask task, Str const *req, Str *resp, Alloc *alloc)
{
    UNUSED(ev);
    UNUSED(alloc);

    log$("task n°{} send us '{}'", task, *req);
    *resp = str$("world");
    return HELLO_SUCCESS;
}

static HelloVTable vtable = {
    hello_hello_handler,
};

int main()
{
    IpcEv ev = {};
    br_ev_init(&ev, alloc_global());

    hello_impl(&ev, &vtable);

    return br_ev_run(&ev);
}
