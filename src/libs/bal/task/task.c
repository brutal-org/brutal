#include <bal/task/task.h>

void bal_task_init(BalTask *task, Str name)
{
    task->name = name;
}

void bal_task_deinit(BalTask *task)
{
    bal_close(task->handle);
}
