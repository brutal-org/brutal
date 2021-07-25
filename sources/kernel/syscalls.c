#include <brutal/host/log.h>
#include <brutal/log.h>
#include <syscalls/syscalls.h>
#include "kernel/domain.h"
#include "kernel/memory.h"
#include "kernel/syscalls.h"
#include "kernel/tasking.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

BrResult sys_log(BrLogArgs *args)
{
    host_log_lock();
    io_write(host_log_writer(), args->message, args->size);
    host_log_unlock();

    return BR_SUCCESS;
}

BrResult sys_map(BrMapArgs *args)
{
    BrResult result = BR_SUCCESS;
    Space *space = nullptr;

    if (args->space == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, args->space, OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    MemObj *mobj = (MemObj *)domain_lookup(task_self()->domain, args->mobj, OBJECT_MEMORY);

    if (mobj == nullptr)
    {
        space_deref(space);
        return BR_BAD_HANDLE;
    }

    auto map_result = space_map(space, mobj, args->offset, args->size, args->vaddr);

    if (map_result.success)
    {
        args->vaddr = UNWRAP(map_result).base;
    }
    else
    {
        result = map_result._error;
    }

    mem_obj_deref(mobj);
    space_deref(space);

    return result;
}

BrResult sys_unmap(BrUnmapArgs *args)
{
    Space *space = nullptr;

    if (args->space == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, args->space, OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    space_unmap(space, (VmmRange){args->vaddr, args->size});

    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_create_task(BrTask *handle, BrCreateTaskArgs *args)
{
    Space *space = nullptr;

    if (args->space == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, args->space, OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    auto task = UNWRAP(task_create(
        str_cast(&args->name),
        space,
        args->caps & task_self()->caps,
        args->flags | BR_TASK_USER));

    domain_add(task_self()->domain, base_cast(task));

    space_deref(space);

    *handle = task->base.handle;

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_create_mem_obj(BrMObj *handle, BrCreateMemObjArgs *args)
{
    MemObj *mobj = nullptr;

    if (args->flags & BR_MOBJ_PMM)
    {
        if (!(task_self()->caps & BR_CAP_PMM))
        {
            return BR_BAD_CAPABILITY;
        }

        mobj = mem_obj_pmm((PmmRange){args->addr, args->size}, MEM_OBJ_NONE);
    }
    else
    {
        auto heap_result = heap_alloc(args->size);

        if (!heap_result.success)
        {
            return heap_result._error;
        }

        mobj = mem_obj_heap(UNWRAP(heap_result), MEM_OBJ_OWNING);
    }

    domain_add(task_self()->domain, base_cast(mobj));
    *handle = mobj->base.handle;
    mem_obj_deref(mobj);

    return BR_SUCCESS;
}

BrResult sys_create_space(BrSpace *handle, BrCreateSpaceArgs *args)
{
    auto space = space_create(args->flags);

    domain_add(task_self()->domain, base_cast(space));
    *handle = space->base.handle;
    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_create(BrCreateArgs *args)
{
    if (!(task_self()->caps & BR_CAP_TASK))
    {
        return BR_BAD_CAPABILITY;
    }

    switch (args->type)
    {
    case BR_OBJECT_TASK:
        return sys_create_task(&args->task_handle, &args->task);

    case BR_OBJECT_SPACE:
        return sys_create_space(&args->space_handle, &args->space);

    case BR_OBJECT_MEMORY:
        return sys_create_mem_obj(&args->mem_obj_handle, &args->mem_obj);

    default:
        return BR_BAD_ARGUMENTS;
    }
}

BrResult sys_start(BrStartArgs *args)
{
    Task *task = (Task *)domain_lookup(task_self()->domain, args->task, OBJECT_TASK);

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    task_start(task, args->ip, args->sp, args->args);

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_exit(BrExitArgs *args)
{
    Task *task = nullptr;

    if (args->task == BR_TASK_SELF)
    {
        task = task_self();
        task_ref(task);
    }
    else
    {
        task = (Task *)domain_lookup(task_self()->domain, args->task, OBJECT_TASK);
    }

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    task_cancel(task, args->exit_value);

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_ipc(BrIpcArgs *args)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult sys_irq(BrIrqArgs *args)
{
    if (!(task_self()->caps & BR_CAP_IRQ))
    {
        return BR_BAD_CAPABILITY;
    }

    return BR_NOT_IMPLEMENTED;
}

BrResult sys_drop(BrDropArgs *args)
{
    Task *task = nullptr;

    if (args->task == BR_TASK_SELF)
    {
        task_ref(task_self());
        task = task_self();
    }
    else
    {
        task = (Task *)domain_lookup(task_self()->domain, args->task, OBJECT_TASK);
    }

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    if (!(task->caps & args->cap))
    {
        task_deref(task);
        return BR_BAD_CAPABILITY;
    }

    task->caps = task->caps & ~args->cap;

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_close(BrCloseArgs *args)
{
    domain_remove(task_self()->domain, args->handle);
    return BR_SUCCESS;
}

#pragma GCC diagnostic pop

typedef BrResult BrSyscallFn();

BrSyscallFn *syscalls[BR_SYSCALL_COUNT] = {
    [BR_SC_LOG] = sys_log,
    [BR_SC_MAP] = sys_map,
    [BR_SC_UNMAP] = sys_unmap,
    [BR_SC_CREATE] = sys_create,
    [BR_SC_START] = sys_start,
    [BR_SC_EXIT] = sys_exit,
    [BR_SC_IPC] = sys_ipc,
    [BR_SC_IRQ] = sys_irq,
    [BR_SC_DROP] = sys_drop,
    [BR_SC_CLOSE] = sys_close,
};

BrResult syscall_dispatch(BrSyscall syscall, BrArg args)
{
    //log("Syscall: {}({#p})", str_cast(br_syscall_to_string(syscall)), args);

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return BR_BAD_SYSCALL;
    }

    task_begin_syscall(task_self());

    auto result = syscalls[syscall](args);

    if (result != BR_SUCCESS)
    {
        log("Syscall: {}({#p}) -> {}", str_cast(br_syscall_to_string(syscall)), args, str_cast(br_result_to_string(result)));
    }

    task_end_syscall(task_self());

    return result;
}
