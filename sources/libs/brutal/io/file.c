#include <brutal/base/attributes.h>
#include <brutal/io/file.h>
#include <embed/file.h>

MaybeError io_file_open(IoFile *self, Str path)
{
    return embed_file_open(&self->embed, path);
}

MaybeError io_file_create(IoFile *self, Str path)
{
    return embed_file_create(&self->embed, path);
}

static IoResult io_file_read_impl(IoFile *self, uint8_t *data, MAYBE_UNUSED size_t offset, size_t size)
{

    return embed_file_read(&self->embed, data, size);
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
    return embed_file_write(&self->embed, data, size);
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
    return embed_file_close(&self->embed);
}
