#include <brutal/host/io.h>
#include <brutal/io/file.h>

MaybeError io_file_open(IoFile *self, Str path)
{
    self->handle = TRY(MaybeError, host_io_file_open(path));
    return SUCCESS;
}

MaybeError io_file_create(IoFile *self, Str path)
{
    self->handle = TRY(MaybeError, host_io_file_create(path));
    return SUCCESS;
}

static IoReadResult io_file_read_impl(IoFileReader *reader, char *data, size_t size)
{
    return host_io_read_file(reader->file->handle, data, size);
}

IoFileReader io_file_read(IoFile *self)
{
    return (IoFileReader){
        .base = {(IoRead *)io_file_read_impl},
        .file = self,
    };
}

static IoWriteResult io_file_write_impl(IoFileWriter *reader, char const *data, size_t size)
{
    return host_io_write_file(reader->file->handle, data, size);
}

IoFileWriter io_file_write(IoFile *self)
{
    return (IoFileWriter){
        .base = {(IoWrite *)io_file_write_impl},
        .file = self,
    };
}

MaybeError io_file_close(IoFile *self)
{
    return host_io_file_close(self->handle);
}
