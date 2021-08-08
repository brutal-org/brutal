#pragma once

#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef enum
{
    IO_STD_IN,
    IO_STD_OUT,
    IO_STD_ERR,
} IoStdChannel;

typedef struct
{
    IoReader base;
    IoStdChannel channel;
} IoStdReader;

typedef struct
{
    IoWriter base;
    IoStdChannel channel;
} IoStdWriter;

IoReader *io_std_in(void);

IoWriter *io_std_out(void);

IoWriter *io_std_err(void);
