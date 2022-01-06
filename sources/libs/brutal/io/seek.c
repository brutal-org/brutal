#include <brutal/io/seek.h>

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

IoResult io_seek(IoSeeker *self, IoSeek off)
{
    return self->seek(self->context, off);
}
