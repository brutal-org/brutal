#include <brutal/host/log.h>
#include <brutal/log.h>
#include <syscalls/syscalls.h>
#include "kernel/domain.h"
#include "kernel/memory.h"
#include "kernel/syscalls.h"
#include "kernel/tasking.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

BrResult sys_noop(void)
{
    return BR_SUCCESS;
}

BrResult sys_log(char const *message, size_t size)
{
    host_log_lock();
    io_write(host_log_writer(), message, size);
    host_log_unlock();

    return BR_SUCCESS;
}

BrResult sys_space(BrSpace *handle, BrSpaceFlags flags)
{
    auto space = space_create();
    domain_add(task_self()->domain, base_cast(space));
    *handle = space->base.handle;
    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_mobj(BrMObj *handle, uintptr_t addr, size_t size, BrMObjFlags flags)
{
    MemoryObject *mobj = nullptr;

    if (flags & BR_MOBJ_PMM)
    {
        mobj = memory_object_create_pmm((PmmRange){addr, size});
    }
    else
    {
        mobj = memory_object_create(size);
    }

    domain_add(task_self()->domain, base_cast(mobj));
    *handle = mobj->base.handle;
    memory_object_deref(mobj);

    return BR_SUCCESS;
}

BrResult sys_map(BrSpace space_handle, BrMObj mobj_handle, uintptr_t vaddr, BrMemFlags flags)
{
    Space *space = nullptr;

    if (space_handle == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, space_handle, OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    MemoryObject *mobj = (MemoryObject *)domain_lookup(task_self()->domain, space_handle, OBJECT_MEMORY);

    if (mobj == nullptr)
    {
        memory_object_deref(mobj);
        return BR_BAD_HANDLE;
    }

    space_map_obj(space, (VmmRange){vaddr, mobj->range.size}, mobj);

    memory_object_deref(mobj);
    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_alloc(BrSpace space_handle, BrMObj mobj_handle, uintptr_t *vaddr, BrMemFlags flags)
{
    Space *space = nullptr;

    if (space_handle == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, space_handle, OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    MemoryObject *mobj = (MemoryObject *)domain_lookup(task_self()->domain, space_handle, OBJECT_MEMORY);

    if (mobj == nullptr)
    {
        memory_object_deref(mobj);
        return BR_BAD_HANDLE;
    }

    auto result = space_alloc_obj(space, mobj);

    *vaddr = result._ok.base;

    memory_object_deref(mobj);
    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_unmap(BrSpace space_handle, uintptr_t vaddr, size_t size)
{
    Space *space = nullptr;

    if (space_handle == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, space_handle, OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    space_unmap(space, (VmmRange){vaddr, size});

    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_task(BrTask *task_handle, BrSpace space_handle, BrTaskArgs const *args, BrTaskFlags flags)
{
    auto space = (Space *)domain_lookup(task_self()->domain, space_handle, OBJECT_SPACE);

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    auto task = UNWRAP(task_create(str_cast("user-task"), space, TASK_USER));

    domain_add(task_self()->domain, base_cast(task));

    space_deref(space);

    *task_handle = task->base.handle;

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_start(BrTask task_handle, uintptr_t ip, uintptr_t sp, BrTaskArgs *args)
{
    Task *task = (Task *)domain_lookup(task_self()->domain, task_handle, OBJECT_TASK);

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    task_start(task, ip, sp, *args);

    task_deref(task);

    return BR_NOT_IMPLEMENTED;
}

BrResult sys_exit(BrTask task_handle, uintptr_t exit_value)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult sys_send(BrTask task_handle, BrMessage const *message, BrTimeout timeout, BrIpcFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult sys_recv(BrTask task_handle, BrMessage *message, BrTimeout timeout, BrIpcFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult sys_irq(BrTask task_handle, BrIrq irq, BrIrqFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult sys_drop(BrTask task_handle, BrCap cap)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult sys_close(BrHandle handle)
{
    domain_remove(task_self()->domain, handle);
    return BR_SUCCESS;
}

#pragma GCC diagnostic pop

typedef BrResult BrSyscallFn();

BrSyscallFn *syscalls[BR_SYSCALL_COUNT] = {
    [BR_SC_NOOP] = sys_noop,
    [BR_SC_LOG] = sys_log,
    [BR_SC_SPACE] = sys_space,
    [BR_SC_MOBJ] = sys_mobj,
    [BR_SC_MAP] = sys_map,
    [BR_SC_ALLOC] = sys_alloc,
    [BR_SC_UNMAP] = sys_unmap,
    [BR_SC_TASK] = sys_task,
    [BR_SC_START] = sys_start,
    [BR_SC_EXIT] = sys_exit,
    [BR_SC_SEND] = sys_send,
    [BR_SC_RECV] = sys_recv,
    [BR_SC_IRQ] = sys_irq,
    [BR_SC_DROP] = sys_drop,
    [BR_SC_CLOSE] = sys_close,
};

Str syscall_to_string(BrSyscall syscall)
{
    static const char *SYSCALL_NAMES[] = {
#define ITER(SYSCALL) #SYSCALL,
        FOREACH_SYSCALLS(ITER)
#undef ITER
    };

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return str_cast("INVALID");
    }

    return str_cast(SYSCALL_NAMES[syscall]);
}

BrResult syscall_dispatch(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5)
{
    log("Syscall: {}({p}, {p}, {p}, {p}, {p})...", syscall_to_string(syscall), arg1, arg2, arg3, arg4, arg5);

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return BR_BAD_SYSCALL;
    }

    return syscalls[syscall](arg1, arg2, arg3, arg4, arg5);
}
