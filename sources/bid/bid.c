
#include <bid/bid.h>
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/io.h>
#include <brutal/host.h>
#include <brutal/log.h>
#include <bid/generator-c.h>

void print_error_pos(Str input, struct bid_error err)
{

    StrFix1 line_char;
    line_char.buffer[0] = '\n';
    line_char.len = 1;

    int line = str_cnt(str_sub(input, 0, err.pos.offset), str_cast(&line_char)); // get the count of '\n' before error pos

    int line_start = str_last(str_sub(input, 0, err.pos.offset), str_cast(&line_char)) + 1;             // get the last '\n' before error pos
    int line_end = str_first(str_sub(input, line_start, input.len), str_cast(&line_char)) + line_start; // get the first '\n' after line start (or after error pos)

    Str line_data = str_sub(input, line_start, line_end);

    print(host_log_writer(), "{} | {}\n", line, line_data);
    print(host_log_writer(), "{} |", line);

    int line_offset = err.pos.offset - line_start; // get the offset of error pos relative to the start of the line

    for (int i = 0; i < line_offset + 1; i++)
    {
        print(host_log_writer(), "-");
    }

    print(host_log_writer(), "^\n");
}

void print_unexpected_token_error(Str input, struct bid_error err)
{
    print_error_pos(input, err);

    panic("expected token: {}", err.specific_information.expected_token.name);
}

void print_error(Str input, struct bid_error err)
{
    log("error: {}", err.message);

    if (err.type == BID_ERROR_TYPE_UNEXPECTED_TOKEN)
    {
        print_unexpected_token_error(input, err);
    }
}

void write_c_file(Str output_path, const struct bid *from)
{
    IoFile output;
    UNWRAP(io_file_create(&output, output_path));

    auto output_writer = io_file_write(&output);
    convert_bid_to_c(from, &output_writer.base);

    io_file_close(&output);
}

BidResult read_bid_file(Str input_file)
{
    IoFile input;
    UNWRAP(io_file_open(&input, input_file));

    Buffer file_buffer;
    buffer_init(&file_buffer, 1000, alloc_global());

    auto read = io_file_read(&input);

    size_t readed = UNWRAP(io_read(&(read.base), ((char *)file_buffer.data), 1000));

    io_file_close(&input);

    Str readed_result = str_cast_n(readed, (char *)file_buffer.data);

    BidResult result = bid_init(readed_result);

    if (!result.success)
    {
        print_error(readed_result, result._error);
        log("error: {} {} on {}", result._error.message, result._error.type, result._error.pos.offset);
        return result;
    }

    return result;
}

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char const *argv[])
{
    if (argc < 3)
    {
        log("bid usage [bid input] [c output]");
        return 0;
    }
    log("converting {} to {}", str_cast(argv[1]), str_cast(argv[2]));

    auto result = read_bid_file(str_cast(argv[1]));

    if (!result.success)
    {
        return -1;
    }

    auto bid = UNWRAP(result);

    write_c_file(str_cast(argv[2]), &bid);

    bid_deinit(&(result._ok));

    return 0;
}
