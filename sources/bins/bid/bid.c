#include <bid/bid.h>
#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        log$("bid usage [bid input] [c output]");
        return 0;
    }

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, str$(argv[1])), "File not found!");

    IoReader source_file_reader = io_file_reader(&source_file);

    Buffer source_buffer;
    buffer_init(&source_buffer, 512, alloc_global());

    IoWriter source_buffer_writer = buffer_writer(&source_buffer);

    io_copy(&source_file_reader, &source_buffer_writer);

    Scan scan;
    scan_init(&scan, buffer_str(&source_buffer));

    BidInterface interface = bid_parse(&scan, alloc_global());

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    IoFile output_file;
    io_file_create(&output_file, str$(argv[2]));

    IoWriter output_file_writer = io_file_writer(&output_file);
    Emit emit;
    emit_init(&emit, &output_file_writer);

    bid2c(&interface, &emit);

    return 0;
}
