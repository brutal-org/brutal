#pragma once

#include <abi/Result.h>
#include <libio/BitReader.h>
#include <libio/Read.h>
#include <libio/ReadCounter.h>
#include <libio/Writer.h>
#include <libutils/Assert.h>
#include <libutils/HashMap.h>
#include <libutils/Prelude.h>
#include <libutils/Vec.h>

namespace Compression
{

struct Inflate
{
private:
    // Fixed huffmann
    Vec<unsigned int> _fixed_alphabet;
    Vec<unsigned int> _fixed_code_bit_lengths;
    Vec<unsigned int> _fixed_dist_alphabet;
    Vec<unsigned int> _fixed_dist_code_bit_lengths;

    // Dynamic huffmann
    Vec<unsigned int> _lit_len_alphabet;
    Vec<unsigned int> _lit_len_code_bit_length;
    Vec<unsigned int> _dist_code_bit_length;
    Vec<unsigned int> _dist_alphabet;

    void build_fixed_huffman_alphabet();
    HjResult build_dynamic_huffman_alphabet(IO::BitReader &input);
    void build_huffman_alphabet(Vec<unsigned int> &alphabet, const Vec<unsigned int> &code_bit_lengths);

    void get_bit_length_count(HashMap<unsigned int, unsigned int> &bit_length_count, const Vec<unsigned int> &code_bit_lengths);
    void get_first_code(HashMap<unsigned int, unsigned int> &firstCodes, HashMap<unsigned int, unsigned int> &bit_length_count);
    void assign_huffman_codes(Vec<unsigned int> &assigned_codes, const Vec<unsigned int> &code_bit_lengths, HashMap<unsigned int, unsigned int> &first_codes);

    HjResult read_blocks(IO::Reader &reader, IO::Writer &uncompressed);

public:
    ResultOr<size_t> perform(IO::Reader &compressed, IO::Writer &uncompressed);
};

} // namespace Compression