#pragma once

/* Valid block types  */
#define DEFLATE_BLOCKTYPE_UNCOMPRESSED 0
#define DEFLATE_BLOCKTYPE_STATIC_HUFFMAN 1
#define DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN 2

/* The maximum number of symbols across all codes  */
#define DEFLATE_MAX_NUM_SYMS 288

// The RFC limits the range of HLIT to 286, but lists HDIST as range
// 1-32, even though distance codes 30 and 31 have no meaning
#define DEFLATE_MAX_NUM_LITLEN_SYMS 286
#define DEFLATE_MAX_NUM_OFFSET_SYMS 30

#define DEFLATE_NUM_PRECODE_SYMS 19
