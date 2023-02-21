#pragma once

#include "traits.h"

typedef enum
{
    IO_CHAN_IN,
    IO_CHAN_OUT,
    IO_CHAN_ERR,
} IoChan;

IoReader io_chan_in(void);

IoWriter io_chan_out(void);

IoWriter io_chan_err(void);

IoReader io_chan_in_locked(void);

IoWriter io_chan_out_locked(void);

IoWriter io_chan_err_locked(void);
