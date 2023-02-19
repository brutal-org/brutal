#include <brutal-fmt>
#include <embed/debug.h>
#include <embed/task.h>

void embed_log_prefix(IoWriter writer)
{
    fmt_format$(writer, "user: {}: ", embed_task_self());
}
