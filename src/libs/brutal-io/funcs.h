#pragma once

#include "buf.h"
#include "traits.h"

/* --- Read Functions ------------------------------------------------------- */

IoReader io_repeat(uint8_t byte);

IoReader io_empty(void);

IoReader io_zero(void);

IoResult io_read(IoReader self, uint8_t *data, size_t size);

#define io_read$(SELF, DATA, SIZE) io_read(io_reader$(SELF), (DATA), (SIZE))

IoResult io_pread(IoRSeek self, uint8_t *data, size_t size, size_t offset);

IoResult io_read_line(IoReader self, Buf *buf, Alloc *alloc);

#define io_read_line$(SELF, BUF, ALLOC) io_read_line(io_reader$(SELF), (BUF), (ALLOC))

IoResult io_read_all(IoReader self, Buf *buf, Alloc *alloc);

#define io_read_all$(SELF, BUF, ALLOC) io_read_all(io_reader$(SELF), (BUF), (ALLOC))

IoResult io_read_byte(IoReader self, uint8_t *c);

#define io_read_byte$(SELF, C) io_read_byte(io_reader$(SELF), (C))

/* --- Write Functions ------------------------------------------------------ */

IoWriter io_sink(void);

IoResult io_write(IoWriter self, uint8_t const *data, size_t size);

#define io_write$(SELF, DATA, SIZE) io_write(io_writer$(SELF), (DATA), (SIZE))

IoResult io_pwrite(IoWSeek self, uint8_t const *data, size_t size, size_t offset);

IoResult io_write_str(IoWriter self, Str str);

#define io_write_str$(SELF, STR) io_write_str(io_writer$(SELF), (STR))

IoResult io_write_byte(IoWriter self, uint8_t c);

#define io_write_byte$(SELF, C) io_write_byte(io_writer$(SELF), (C))

/* --- Seek Functions ------------------------------------------------------- */

IoResult io_seek(IoSeeker self, IoSeek off);

#define io_seek$(SELF, OFF) io_seek(io_seeker$(SELF), (OFF))

IoResult io_tell(IoSeeker seeker);

#define io_tell$(SELF) io_tell(io_seeker$(SELF))

IoResult io_size(IoSeeker seeker);

#define io_size$(SELF) io_size(io_seeker$(SELF))

IoResult io_skip(IoReader from, size_t size);

#define io_skip$(FROM, SIZE) io_skip(io_reader$(FROM), (SIZE))

/* --- Copy Functions ------------------------------------------------------- */

IoResult io_copy(IoReader from, IoWriter to);

#define io_copy$(FROM, TO) io_copy(io_reader$(FROM), io_writer$(TO))

IoResult io_copy_n(IoReader from, IoWriter to, size_t size);

#define io_copy_n$(FROM, TO, SIZE) io_copy_n(io_reader$(FROM), io_writer$(TO), (SIZE))
