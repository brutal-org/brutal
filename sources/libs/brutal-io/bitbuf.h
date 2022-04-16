#pragma once

#include <brutal-base>

typedef uint_fast64_t BitBuf;

#define BITBUF_NBITS (8 * sizeof(BitBuf))

/**
  @brief The maximum number of bits that can be ensured in the bitbuffer variable,
*/
#define MAX_ENSURE (BITBUF_NBITS - 7)
