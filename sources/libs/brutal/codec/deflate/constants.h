#pragma once

/* Valid block types  */
#define DEFLATE_BLOCKTYPE_UNCOMPRESSED 0
#define DEFLATE_BLOCKTYPE_STATIC_HUFFMAN 1
#define DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN 2

/* The maximum number of symbols across all codes  */
#define DEFLATE_MAX_NUM_SYMS 288

#define DEFLATE_MAX_NUM_LITLEN_SYMS 286
#define DEFLATE_MAX_NUM_OFFSET_SYMS 30

#define DEFLATE_NUM_PRECODE_SYMS 19

/*
 * Each ENOUGH number is the maximum number of decode table entries that may be
 * required for the corresponding Huffman code, including the main table and all
 * subtables.  Each number depends on three parameters:
 *
 *	(1) the maximum number of symbols in the code (DEFLATE_NUM_*_SYMS)
 *	(2) the number of main table bits (the TABLEBITS numbers defined above)
 *	(3) the maximum allowed codeword length (DEFLATE_MAX_*_CODEWORD_LEN)
 *
 * The ENOUGH numbers were computed using the utility program 'enough' from
 * zlib.  This program enumerates all possible relevant Huffman codes to find
 * the worst-case usage of decode table entries.
 */
#define ENOUGH_LENS 852
#define ENOUGH_DISTS 592
#define ENOUGH (ENOUGH_LENS + ENOUGH_DISTS)

#define bsr32(n) (31 - __builtin_clz(n))
#define bsr64(n) (63 - __builtin_clzll(n))
#define bsf32(n) __builtin_ctz(n)
#define bsf64(n) __builtin_ctzll(n)
