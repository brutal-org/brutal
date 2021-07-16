
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/bid/bid_convert_c.h>
#include <brutal/io.h>
#include <brutal/io/buffer.h>
#include <brutal/io/scan.h>
#include <brutal/log.h>
#include "brutal/alloc/global.h"
#include "brutal/host/log.h"
#include "brutal/text/str.h"

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char const *argv[])
{
    if (argc < 3)
    {
        log("bid usage [bid input] [c output]");
        return 0;
    }
    log("converting {} to {}", str_cast(argv[1]), str_cast(argv[2]));

    HostIoFileHandle handle = UNWRAP(host_io_file_open(str_cast(argv[1])));

    Buffer file_buffer;
    buffer_init(&file_buffer, 1000, alloc_global());

    size_t readed = UNWRAP(host_io_read_file(handle, (char *)file_buffer.data, 1000));
    host_io_file_close(handle);

    Str readed_result = str_cast_n(readed, (char *)file_buffer.data);

    BidResult result = init_bid(readed_result);

    if (!result.success)
    {
        log("error: {} {} on {}", result._error.message, result._error.type, result._error.pos.offset);
        return 1;
    }

    print_ast_node_recursive((result._ok.root_ast));

    convert_bid_to_c(&result._ok, host_log_writer());
    destroy_bid(&(result._ok));

    // {keyword } {action} {;}
    // {keyword } {action} {;}
    // ...

    return 0;
}
