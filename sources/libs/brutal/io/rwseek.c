#include <brutal/io/rwseek.h>

IoResult io_pread(IoRwSeek self, uint8_t *data, size_t size, size_t offset)
{
    io_seek(self.seeker, io_seek_from_start(offset));
    return io_read(self.reader, data, size);
}

IoResult io_pwrite(IoRwSeek self, uint8_t const *data, size_t size, size_t offset)
{
    io_seek(self.seeker, io_seek_from_start(offset));
    return io_write(self.writer, data, size);
}
