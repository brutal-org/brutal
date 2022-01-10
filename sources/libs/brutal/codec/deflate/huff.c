#include <brutal/codec/deflate/constants.h>
#include <brutal/codec/deflate/huff.h>

#define HUFFDEC_SUBTABLE_POINTER 0x80000000

bool build_decode_table(uint32_t decode_table[],
                        const uint8_t lens[],
                        const unsigned num_syms,
                        const uint32_t decode_results[],
                        const unsigned table_bits,
                        const unsigned max_codeword_len,
                        uint16_t *sorted_syms)
{
    unsigned len_counts[DEFLATE_MAX_CODEWORD_LEN + 1];
    unsigned offsets[DEFLATE_MAX_CODEWORD_LEN + 1];
    unsigned sym;             /* current symbol */
    unsigned codeword;        /* current codeword, bit-reversed */
    unsigned len;             /* current codeword length in bits */
    unsigned count;           /* num codewords remaining with this length */
    uint32_t codespace_used;  /* codespace used out of '2^max_codeword_len' */
    unsigned cur_table_end;   /* end index of current table */
    unsigned subtable_prefix; /* codeword prefix of current subtable */
    unsigned subtable_start;  /* start index of current subtable */
    unsigned subtable_bits;   /* log2 of current subtable length */

    /* Count how many codewords have each length, including 0. */
    for (len = 0; len <= max_codeword_len; len++)
        len_counts[len] = 0;
    for (sym = 0; sym < num_syms; sym++)
        len_counts[lens[sym]]++;

    /*
	 * Sort the symbols primarily by increasing codeword length and
	 * secondarily by increasing symbol value; or equivalently by their
	 * codewords in lexicographic order, since a canonical code is assumed.
	 *
	 * For efficiency, also compute 'codespace_used' in the same pass over
	 * 'len_counts[]' used to build 'offsets[]' for sorting.
	 */

    /* Ensure that 'codespace_used' cannot overflow. */
    static_assert(UINT32_MAX / (1U << (DEFLATE_MAX_CODEWORD_LEN - 1)) >=
                      DEFLATE_MAX_NUM_SYMS,
                  "Codespace can overflow");

    offsets[0] = 0;
    offsets[1] = len_counts[0];
    codespace_used = 0;
    for (len = 1; len < max_codeword_len; len++)
    {
        offsets[len + 1] = offsets[len] + len_counts[len];
        codespace_used = (codespace_used << 1) + len_counts[len];
    }
    codespace_used = (codespace_used << 1) + len_counts[len];

    for (sym = 0; sym < num_syms; sym++)
        sorted_syms[offsets[lens[sym]]++] = sym;

    sorted_syms += offsets[0]; /* Skip unused symbols */

    /* lens[] is done being used, so we can write to decode_table[] now. */

    /*
	 * Check whether the lengths form a complete code (exactly fills the
	 * codespace), an incomplete code (doesn't fill the codespace), or an
	 * overfull code (overflows the codespace).  A codeword of length 'n'
	 * uses proportion '1/(2^n)' of the codespace.  An overfull code is
	 * nonsensical, so is considered invalid.  An incomplete code is
	 * considered valid only in two specific cases; see below.
	 */

    /* overfull code? */
    if (codespace_used > (1U << max_codeword_len))
        return false;

    /* incomplete code? */
    if (codespace_used < (1U << max_codeword_len))
    {
        uint32_t entry;
        unsigned i;

        if (codespace_used == 0)
        {
            /*
			 * An empty code is allowed.  This can happen for the
			 * offset code in DEFLATE, since a dynamic Huffman block
			 * need not contain any matches.
			 */

            /* sym=0, len=1 (arbitrary) */
            entry = decode_results[0] | 1;
        }
        else
        {
            /*
			 * Allow codes with a single used symbol, with codeword
			 * length 1.  The DEFLATE RFC is unclear regarding this
			 * case.  What zlib's decompressor does is permit this
			 * for the litlen and offset codes and assume the
			 * codeword is '0' rather than '1'.  We do the same
			 * except we allow this for precodes too, since there's
			 * no convincing reason to treat the codes differently.
			 * We also assign both codewords '0' and '1' to the
			 * symbol to avoid having to handle '1' specially.
			 */
            if (codespace_used != (1U << (max_codeword_len - 1)) ||
                len_counts[1] != 1)
                return false;
            entry = decode_results[*sorted_syms] | 1;
        }
        /*
		 * Note: the decode table still must be fully initialized, in
		 * case the stream is malformed and contains bits from the part
		 * of the codespace the incomplete code doesn't use.
		 */
        for (i = 0; i < (1U << table_bits); i++)
            decode_table[i] = entry;
        return true;
    }

    /*
	 * The lengths form a complete code.  Now, enumerate the codewords in
	 * lexicographic order and fill the decode table entries for each one.
	 *
	 * First, process all codewords with len <= table_bits.  Each one gets
	 * '2^(table_bits-len)' direct entries in the table.
	 *
	 * Since DEFLATE uses bit-reversed codewords, these entries aren't
	 * consecutive but rather are spaced '2^len' entries apart.  This makes
	 * filling them naively somewhat awkward and inefficient, since strided
	 * stores are less cache-friendly and preclude the use of word or
	 * vector-at-a-time stores to fill multiple entries per instruction.
	 *
	 * To optimize this, we incrementally double the table size.  When
	 * processing codewords with length 'len', the table is treated as
	 * having only '2^len' entries, so each codeword uses just one entry.
	 * Then, each time 'len' is incremented, the table size is doubled and
	 * the first half is copied to the second half.  This significantly
	 * improves performance over naively doing strided stores.
	 *
	 * Note that some entries copied for each table doubling may not have
	 * been initialized yet, but it doesn't matter since they're guaranteed
	 * to be initialized later (because the Huffman code is complete).
	 */
    codeword = 0;
    len = 1;
    while ((count = len_counts[len]) == 0)
        len++;
    cur_table_end = 1U << len;
    while (len <= table_bits)
    {
        /* Process all 'count' codewords with length 'len' bits. */
        do
        {
            unsigned bit;

            /* Fill the first entry for the current codeword. */
            decode_table[codeword] =
                decode_results[*sorted_syms++] | len;

            if (codeword == cur_table_end - 1)
            {
                /* Last codeword (all 1's) */
                for (; len < table_bits; len++)
                {
                    memcpy(&decode_table[cur_table_end],
                           decode_table,
                           cur_table_end *
                               sizeof(decode_table[0]));
                    cur_table_end <<= 1;
                }
                return true;
            }
            /*
			 * To advance to the lexicographically next codeword in
			 * the canonical code, the codeword must be incremented,
			 * then 0's must be appended to the codeword as needed
			 * to match the next codeword's length.
			 *
			 * Since the codeword is bit-reversed, appending 0's is
			 * a no-op.  However, incrementing it is nontrivial.  To
			 * do so efficiently, use the 'bsr' instruction to find
			 * the last (highest order) 0 bit in the codeword, set
			 * it, and clear any later (higher order) 1 bits.  But
			 * 'bsr' actually finds the highest order 1 bit, so to
			 * use it first flip all bits in the codeword by XOR'ing
			 * it with (1U << len) - 1 == cur_table_end - 1.
			 */
            bit = 1U << bsr32(codeword ^ (cur_table_end - 1));
            codeword &= bit - 1;
            codeword |= bit;
        } while (--count);

        /* Advance to the next codeword length. */
        do
        {
            if (++len <= table_bits)
            {
                mem_cpy(&decode_table[cur_table_end],
                        decode_table,
                        cur_table_end * sizeof(decode_table[0]));
                cur_table_end <<= 1;
            }
        } while ((count = len_counts[len]) == 0);
    }

    /* Process codewords with len > table_bits.  These require subtables. */
    cur_table_end = 1U << table_bits;
    subtable_prefix = -1;
    subtable_start = 0;
    for (;;)
    {
        uint32_t entry;
        unsigned i;
        unsigned stride;
        unsigned bit;

        /*
		 * Start a new subtable if the first 'table_bits' bits of the
		 * codeword don't match the prefix of the current subtable.
		 */
        if ((codeword & ((1U << table_bits) - 1)) != subtable_prefix)
        {
            subtable_prefix = (codeword & ((1U << table_bits) - 1));
            subtable_start = cur_table_end;
            /*
			 * Calculate the subtable length.  If the codeword has
			 * length 'table_bits + n', then the subtable needs
			 * '2^n' entries.  But it may need more; if fewer than
			 * '2^n' codewords of length 'table_bits + n' remain,
			 * then the length will need to be incremented to bring
			 * in longer codewords until the subtable can be
			 * completely filled.  Note that because the Huffman
			 * code is complete, it will always be possible to fill
			 * the subtable eventually.
			 */
            subtable_bits = len - table_bits;
            codespace_used = count;
            while (codespace_used < (1U << subtable_bits))
            {
                subtable_bits++;
                codespace_used = (codespace_used << 1) +
                                 len_counts[table_bits + subtable_bits];
            }
            cur_table_end = subtable_start + (1U << subtable_bits);

            /*
			 * Create the entry that points from the main table to
			 * the subtable.  This entry contains the index of the
			 * start of the subtable and the number of bits with
			 * which the subtable is indexed (the log base 2 of the
			 * number of entries it contains).
			 */
            decode_table[subtable_prefix] =
                HUFFDEC_SUBTABLE_POINTER |
                HUFFDEC_RESULT_ENTRY(subtable_start) |
                subtable_bits;
        }

        /* Fill the subtable entries for the current codeword. */
        entry = decode_results[*sorted_syms++] | (len - table_bits);
        i = subtable_start + (codeword >> table_bits);
        stride = 1U << (len - table_bits);
        do
        {
            decode_table[i] = entry;
            i += stride;
        } while (i < cur_table_end);

        /* Advance to the next codeword. */
        if (codeword == (1U << len) - 1) /* last codeword (all 1's)? */
            return true;
        bit = 1U << bsr32(codeword ^ ((1U << len) - 1));
        codeword &= bit - 1;
        codeword |= bit;
        count--;
        while (count == 0)
            count = len_counts[++len];
    }
}