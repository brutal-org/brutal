#pragma once

#include <brutal-io>

typedef struct
{
    uint32_t s1;
    uint32_t s2;
    IoWriter underlying;
} Adler32;

void adler32_init(Adler32 *self, IoWriter underlying);

IoWriter adler32_writer(Adler32 *self);

uint32_t adler32_get(Adler32 *self);

uint32_t adler32(void const *buf, size_t len);
