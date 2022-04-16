#include <brutal-fmt>
#include <embed/debug.h>

#include "kernel/cpu.h"

void embed_log_prefix(IoWriter writer)
{
    fmt_format$(writer, "cpu{}: ", cpu_self_id());

    if (task_self())
    {
        fmt_format$(writer, "{}: ", task_self()->id);
    }
}
