#pragma once

#include <brutal/io/base.h>

typedef IoResult IoReadFn(void *context, uint8_t *data, size_t size);
typedef IoResult IoWriteFn(void *context, uint8_t const *data, size_t size);
typedef IoResult IoSeekFn(void *context, IoSeek off);

typedef IoResult IoMapFn(void *context, void **ptr, size_t size, IoMapHint hints);
typedef IoResult IoUnmapFn(void *context, void *ptr);

/* --- Base Traits ---------------------------------------------------------- */

typedef struct
{
    IoReadFn *read;
    void *context;
} IoReader;

typedef struct
{
    IoWriteFn *write;
    void *context;
} IoWriter;

typedef struct
{
    IoSeekFn *seek;
    void *context;
} IoSeeker;

typedef struct
{
    IoMapFn *map;
    IoUnmapFn *unmap;

    void *context;
} IoMapper;

/* --- Compound Traits ------------------------------------------------------ */

typedef struct
{
    IoReader reader;
    IoWriter writer;
} IoDuplex;

typedef struct
{
    IoReader reader;
    IoSeeker seeker;
} IoRSeek;

typedef struct
{
    IoWriter writer;
    IoSeeker seeker;
} IoWSeek;
