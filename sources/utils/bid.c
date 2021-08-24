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

    log$("converting {} to {}", str$(argv[1]), str$(argv[2]));

    IoFile source_file;
    io_file_open(&source_file, str$(argv[1]));

    IoFileReader source_file_reader = io_file_read(&source_file);

    Buffer source_buffer;
    buffer_init(&source_buffer, 512, alloc_global());

    IoBufferWriter source_buffer_writer = io_buffer_write(&source_buffer);

    io_copy(base$(&source_file_reader), base$(&source_buffer_writer));

    Scan scan;
    scan_init(&scan, buffer_str(&source_buffer));

    BidInterface interface = bid_parse(&scan, alloc_global());

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    IoFile output_file;
    io_file_create(&output_file, str$(argv[2]));

    IoFileWriter output_file_writer = io_file_write(&output_file);

    bid2c(&interface, base$(&output_file_writer));

    return 0;
}
