#pragma once

#include <brutal-io>

/**
  @brief Decompress a raw deflate stream
  @param in The input data to compress
  @param in_len The size of the input data
  @param out The output buffer where we can store our data
  @param out_len The size of the output buffer
  @return The number of bytes written to the \p out buffer (uncompressed size)
*/
IoResult deflate_decompress_data(uint8_t const *in, size_t in_len, uint8_t *out, size_t out_len);

/**
  @brief Decompress a raw deflate stream
  @param writer The destination where we write the uncompressed data
  @param reader The source where we read the compressed data from
  @return The number of bytes written to the \p writer stream (uncompressed size)
*/
IoResult deflate_decompress_stream(IoWriter writer, IoReader reader);
