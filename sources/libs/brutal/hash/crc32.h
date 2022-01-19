#pragma once

#include <brutal/io/traits.h>

typedef struct
{
    uint32_t crc;
    IoWriter underlying;
} Crc32;

void crc32_init(Crc32 *self, IoWriter underlying);

IoWriter crc32_writer(Crc32 *self);

uint32_t crc32_get(Crc32 *self);

uint32_t crc32(void const *buf, size_t len);
