#pragma once

#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef struct
{
    IoReader reader;
    IoWriter writer;
} IoDuplex;
