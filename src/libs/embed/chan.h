#pragma once

#include <brutal-io/chan.h>

IoResult embed_chan_read(IoChan channel, uint8_t *data, size_t size);

IoResult embed_chan_write(IoChan channel, uint8_t const *data, size_t size);
