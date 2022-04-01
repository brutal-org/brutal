#include <abi/Result.h>
#include <libmath/MinMax.h>
#include <string.h>

#include "system/node/File.h"
#include "system/node/Handle.h"

FsFile::FsFile() : FsNode(HJ_FILE_TYPE_REGULAR)
{
    _buffer = (char *)malloc(512);
    _buffer_allocated = 512;
    _buffer_size = 0;
}

FsFile::~FsFile()
{
    free(_buffer);
}

HjResult FsFile::open(FsHandle &handle)
{
    if (handle.has_flag(HJ_OPEN_TRUNC))
    {
        free(_buffer);
        _buffer = (char *)malloc(512);
        _buffer_allocated = 512;
        _buffer_size = 0;
    }

    return SUCCESS;
}

size_t FsFile::size()
{
    return _buffer_size;
}

ResultOr<size_t> FsFile::read(FsHandle &handle, void *buffer, size_t size)
{
    size_t read = 0;

    if (handle.offset() <= _buffer_size)
    {
        read = MIN(_buffer_size - handle.offset(), size);
        memcpy(buffer, (char *)_buffer + handle.offset(), read);
    }

    return read;
}

ResultOr<size_t> FsFile::write(FsHandle &handle, const void *buffer, size_t size)
{
    if ((handle.offset() + size) > _buffer_allocated)
    {
        _buffer = (char *)realloc(_buffer, handle.offset() + size);
        _buffer_allocated = handle.offset() + size;
    }

    _buffer_size = MAX(handle.offset() + size, _buffer_size);
    memcpy((char *)(_buffer) + handle.offset(), buffer, size);

    return size;
}
