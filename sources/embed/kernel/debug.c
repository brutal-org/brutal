#include <embed/debug.h>
#include <kernel/cpu.h>

void embed_log_prefix(IoWriter *writer)
{
    print(writer, "cpu{}: ", cpu_self_id());

    if (task_self())
    {
        print(writer, "{}: ", task_self()->id);
    }
}
