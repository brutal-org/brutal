#pragma once

#include "base.h"

typedef IoResult IoReadFn(void *context, uint8_t *data, size_t size);
typedef IoResult IoWriteFn(void *context, uint8_t const *data, size_t size);
typedef IoResult IoSeekFn(void *context, IoSeek off);

typedef IoResult IoMapFn(void *context, void **ptr, size_t size, IoMapHint hints);
typedef IoResult IoUnmapFn(void *context, void *ptr);

typedef struct
{
    IoReadFn *read;
    void *context;
} IoReader;

#define io_reader$(SELF) \
    (IoReader) { .read = (SELF).read, .context = (SELF).context }

typedef struct
{
    IoWriteFn *write;
    void *context;
} IoWriter;

#define io_writer$(SELF) \
    (IoWriter) { .write = (SELF).write, .context = (SELF).context }

typedef struct
{
    IoSeekFn *seek;
    void *context;
} IoSeeker;

#define io_seeker$(SELF) \
    (IoSeeker) { .seek = (SELF).seek, .context = (SELF).context }

typedef struct
{
    IoMapFn *map;
    IoUnmapFn *unmap;
    void *context;
} IoMapper;

typedef struct
{
    IoReadFn *read;
    IoWriteFn *write;
    void *context;
} IoDuplex;

#define io_duplex$(READER, WRITER) \
    (IoDuplex) { .read = (READER).read, .write = (WRITER).write, .context = (READER).context }

typedef struct
{
    IoReadFn *read;
    IoSeekFn *seek;
    void *context;
} IoRSeek;

#define io_rseek$(READER, SEEKER) \
    (IoRSeek) { .read = (READER).read, .seek = (SEEKER).seek, .context = (READER).context }

typedef struct
{
    IoWriteFn *write;
    IoSeekFn *seek;
    void *context;
} IoWSeek;

#define io_wseek$(WRITER, SEEKER) \
    (IoWSeek) { .write = (WRITER).write, .seek = (SEEKER).seek, .context = (WRITER).context }
