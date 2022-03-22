#include <embed/debug.h>

void embed_log_prefix(IoWriter writer)
{
    io_fmt$(writer, "efi: ");
}
