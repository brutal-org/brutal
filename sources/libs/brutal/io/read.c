#include <brutal/io/read.h>

IoResult io_read(IoReader self, uint8_t *data, size_t size)
{
    return self.read(self.context, data, size);
}
