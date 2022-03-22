#include <embed/debug.h>
#include <kernel/cpu.h>

void embed_log_prefix(IoWriter writer)
{
    io_fmt$(writer, "cpu{}: ", cpu_self_id());

    if (task_self())
    {
        io_fmt$(writer, "{}: ", task_self()->id);
    }
}
