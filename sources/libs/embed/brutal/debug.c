#include <embed/debug.h>
#include <embed/task.h>

void embed_log_prefix(IoWriter writer)
{
    io_print$(writer, "user: {}: ", embed_task_self());
}
