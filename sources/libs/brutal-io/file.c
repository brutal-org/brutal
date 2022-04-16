#include <embed/file.h>

#include "file.h"

MaybeError io_file_init(IoFile *self, Str path, FileInitFlags flags)
{
    return embed_file_open(self, path, flags);
}

MaybeError io_file_view(IoFile *self, Str path)
{
    return io_file_init(self, path, IO_FILE_READ);
}

MaybeError io_file_create(IoFile *self, Str path)
{
    return embed_file_create(self, path);
}

MaybeError io_file_edit(IoFile *self, Str path)
{
    return io_file_init(self, path, IO_FILE_READ | IO_FILE_WRITE);
}

IoReader io_file_reader(IoFile *self)
{
    return (IoReader){
        .read = (IoReadFn *)embed_file_read,
        .context = self,
    };
}

IoWriter io_file_writer(IoFile *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)embed_file_write,
        .context = self,
    };
}

IoSeeker io_file_seeker(IoFile *self)
{
    return (IoSeeker){
        .seek = (IoSeekFn *)embed_file_seek,
        .context = self,
    };
}

IoDuplex io_file_duplex(IoFile *self)
{
    return (IoDuplex){
        .read = (IoReadFn *)embed_file_read,
        .write = (IoWriteFn *)embed_file_write,
        .context = self,
    };
}

IoRSeek io_file_rseek(IoFile *self)
{
    return (IoRSeek){
        .read = (IoReadFn *)embed_file_read,
        .seek = (IoSeekFn *)embed_file_seek,
        .context = self,
    };
}

IoWSeek io_file_wseek(IoFile *self)
{
    return (IoWSeek){
        .write = (IoWriteFn *)embed_file_write,
        .seek = (IoSeekFn *)embed_file_seek,
        .context = self,
    };
}

MaybeError io_file_close(IoFile *self)
{
    return embed_file_close(self);
}
