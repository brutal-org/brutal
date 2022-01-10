#pragma once

/* Valid block types  */
#define DEFLATE_BLOCKTYPE_UNCOMPRESSED 0
#define DEFLATE_BLOCKTYPE_STATIC_HUFFMAN 1
#define DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN 2

/* Minimum and maximum supported match lengths (in bytes)  */
#define DEFLATE_MIN_MATCH_LEN 3
#define DEFLATE_MAX_MATCH_LEN 258

/* The maximum number of symbols across all codes  */
#define DEFLATE_MAX_NUM_SYMS 288

/* The maximum codeword length across all codes  */
#define DEFLATE_MAX_CODEWORD_LEN 15

/* Maximum possible overrun when decoding codeword lengths  */
#define DEFLATE_MAX_LENS_OVERRUN 137

/* Number of symbols in each Huffman code.  Note: for the literal/length
 * and offset codes, these are actually the maximum values; a given block
 * might use fewer symbols.  */
#define DEFLATE_NUM_PRECODE_SYMS 19
#define DEFLATE_NUM_LITLEN_SYMS 288
#define DEFLATE_NUM_OFFSET_SYMS 32

#define bsr32(n) (31 - __builtin_clz(n))
#define bsr64(n) (63 - __builtin_clzll(n))
#define bsf32(n) __builtin_ctz(n)
#define bsf64(n) __builtin_ctzll(n)
