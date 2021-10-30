#include <brutal/base/attributes.h>
#include <brutal/io/file.h>
#include <embed/io.h>

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

static IoResult io_file_read_impl(IoFile *self, uint8_t *data, MAYBE_UNUSED size_t offset, size_t size)
{

    return host_io_read_file(self->handle, data, size);
}

IoReader io_file_reader(IoFile *self)
{
    return (IoReader){
        .read = (IoRead *)io_file_read_impl,
        .context = self,
    };
}

static IoResult io_file_write_impl(IoFile *self, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    return host_io_write_file(self->handle, data, size);
}

IoWriter io_file_writer(IoFile *self)
{
    return (IoWriter){
        .write = (IoWrite *)io_file_write_impl,
        .context = self,
    };
}

MaybeError io_file_close(IoFile *self)
{
    return host_io_file_close(self->handle);
}
