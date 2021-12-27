#include <embed/debug.h>

void embed_log_prefix(IoWriter *writer)
{
    print(writer, "efi: ");
}
