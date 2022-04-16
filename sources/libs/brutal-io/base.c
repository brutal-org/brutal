#include <brutal-debug>

#include "base.h"

IoSeek io_seek_from_start(SSize position)
{
    return (IoSeek){IO_WHENCE_START, position};
}

IoSeek io_seek_from_current(SSize position)
{
    return (IoSeek){IO_WHENCE_CURRENT, position};
}

IoSeek io_seek_from_end(SSize position)
{
    return (IoSeek){IO_WHENCE_END, position};
}

SSize io_seek_eval(IoSeek seek, SSize current, SSize size)
{
    switch (seek.whence)
    {
    case IO_WHENCE_START:
        return seek.position;

    case IO_WHENCE_CURRENT:
        return current + seek.position;

    case IO_WHENCE_END:
        return size + seek.position;

    default:
        panic$("Unknow whence {}", seek.whence);
    }
}
