#pragma once

#include <brutal-io>

typedef struct
{
    uint32_t crc;
    size_t count;
    IoWriter base_writer;
    IoReader base_reader;
} Crc32;

void crc32_init_writer(Crc32 *self, IoWriter base);
void crc32_init_reader(Crc32 *self, IoReader base);

IoWriter crc32_writer(Crc32 *self);
IoReader crc32_reader(Crc32 *self);

uint32_t crc32_get(Crc32 *self);
size_t crc32_count(Crc32 *self);

uint32_t crc32(void const *buf, size_t len);
