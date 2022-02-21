#include <brutal/base/attributes.h>
#include <brutal/io/file.h>
#include <embed/file.h>

MaybeError io_file_open(IoFile *self, Str path, FileOpenFlags flags)
{
    return embed_file_open(self, path, flags);
}

MaybeError io_file_create(IoFile *self, Str path)
{
    return embed_file_create(self, path);
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
        .reader = io_file_reader(self),
        .writer = io_file_writer(self),
    };
}

IoRSeek io_file_rseek(IoFile *self)
{
    return (IoRSeek){
        .reader = io_file_reader(self),
        .seeker = io_file_seeker(self),
    };
}

IoWSeek io_file_wseek(IoFile *self)
{
    return (IoWSeek){
        .writer = io_file_writer(self),
        .seeker = io_file_seeker(self),
    };
}

MaybeError io_file_close(IoFile *self)
{
    return embed_file_close(self);
}
