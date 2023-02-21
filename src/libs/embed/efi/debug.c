#include <brutal/fmt/funcs.h>
#include <embed/debug.h>

void embed_log_prefix(IoWriter writer)
{
    fmt_format$(writer, "efi: ");
}
