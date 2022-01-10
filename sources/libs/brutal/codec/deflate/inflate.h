#pragma once

#include <brutal/io.h>

/**
  @brief Decompress an entire stream
  @param writer The destination where we write the uncompressed data
  @param reader The source where we read the compressed data from
  @return The number of bytes written to the \p writer stream (uncompressed size)
*/
IoResult deflate_decompress_stream(IoWriter *writer, IoReader *reader);
