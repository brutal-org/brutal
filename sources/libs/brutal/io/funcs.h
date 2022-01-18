#pragma once

#include <brutal/io/buf.h>
#include <brutal/io/traits.h>

/* --- Read Functions ------------------------------------------------------- */

IoReader io_repeat(uint8_t byte);

IoReader io_empty(void);

IoReader io_zero(void);

IoResult io_read(IoReader self, uint8_t *data, size_t size);

IoResult io_pread(IoRSeek self, uint8_t *data, size_t size, size_t offset);

IoResult io_read_line(IoReader self, Buf *buf, Alloc *alloc);

IoResult io_read_all(IoReader self, Buf *buf, Alloc *alloc);

IoResult io_read_byte(IoReader self, uint8_t *c);

/* --- Write Functions ------------------------------------------------------ */

IoWriter io_sink(void);

IoResult io_write(IoWriter self, uint8_t const *data, size_t size);

IoResult io_pwrite(IoWSeek self, uint8_t const *data, size_t size, size_t offset);

IoResult io_write_str(IoWriter self, Str str);

IoResult io_write_byte(IoWriter self, uint8_t c);

/* --- Seek Functions ------------------------------------------------------- */

IoResult io_seek(IoSeeker self, IoSeek off);

IoResult io_tell(IoSeeker seeker);

IoResult io_size(IoSeeker seeker);

IoResult io_skip(IoReader from, size_t size);

/* --- Copy Functions ------------------------------------------------------- */

IoResult io_copy(IoReader from, IoWriter to);

IoResult io_copy_n(IoReader from, IoWriter to, size_t size);
