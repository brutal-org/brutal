#pragma once

#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef enum
{
    IO_CHAN_IN,
    IO_CHAN_OUT,
    IO_CHAN_ERR,
} IoChan;

IoReader io_chan_in(void);

IoWriter io_chan_out(void);

IoWriter io_chan_err(void);
