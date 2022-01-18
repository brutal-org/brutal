#include <brutal/io/base.h>

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
