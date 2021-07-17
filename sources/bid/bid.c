
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/bid/bid_convert_c.h>
#include <brutal/io.h>
#include <brutal/io/buffer.h>
#include <brutal/io/scan.h>
#include <brutal/log.h>
#include "brutal/alloc/global.h"
#include "brutal/host/io.h"
#include "brutal/host/log.h"
#include "brutal/text/str.h"

size_t get_line_count_from_input(size_t offset, Str input, int *line_offset)
{

    size_t count = 0;
    for (size_t i = 0; i < offset; i++)
    {
        (*line_offset)++;
        if (input.buffer[i] == '\n')
        {
            (*line_offset) = 0;
            count++;
        }
    }
    return count;
}

Str get_line(size_t line, Str input)
{
    size_t count = 0;
    size_t start = 0;
    size_t end = 0;

    for (size_t i = 0; i < input.len; i++)
    {
        if (input.buffer[i] == '\n')
        {
            count++;
            if (count == line && start == 0)
            {
                start = i + 1;
            }
            else if (start != 0)
            {
                end = i;
                break;
            }
        }
    }

    if (end == 0 && start != 0)
    {
        end = input.len;
    }

    return str_cast_n(end - start, input.buffer + start);
}

void print_error_pos(Str input, struct bid_error err)
{
    int line_offset = 0;
    int line = get_line_count_from_input(err.pos.offset, input, &line_offset);

    print(host_log_writer(), "{} | {} \n", line, get_line(line, input));
    print(host_log_writer(), "{} |", line);

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

    BidResult result = init_bid(readed_result);

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

    destroy_bid(&(result._ok));

    return 0;
}
