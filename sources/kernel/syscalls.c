#include <brutal/host/log.h>
#include <brutal/log.h>
#include <syscalls/syscalls.h>
#include "kernel/domain.h"
#include "kernel/global.h"
#include "kernel/interrupts.h"
#include "kernel/memory.h"
#include "kernel/sched.h"
#include "kernel/syscalls.h"
#include "kernel/task.h"

BrResult sys_log(BrLogArgs *args)
{
    host_log_lock();
    print(host_log_writer(), "cpu{}: {}({}): ", cpu_self_id(), str$(&task_self()->name), task_self()->handle);
    io_write(host_log_writer(), (uint8_t *)args->message, args->size);
    host_log_unlock();

    return BR_SUCCESS;
}

BrResult sys_map(BrMapArgs *args)
{
    Space *space = nullptr;
    MemObj *mem_obj = nullptr;
    BrResult result = BR_SUCCESS;

    if (args->space == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, args->space, BR_OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        result = BR_BAD_HANDLE;
        goto cleanup_and_return;
    }

    if (args->mem_obj == BR_MEM_OBJ_GINFO)
    {
        mem_obj = global_create_obj();
    }
    else
    {
        mem_obj = (MemObj *)domain_lookup(task_self()->domain, args->mem_obj, BR_OBJECT_MEMORY);
    }

    if (mem_obj == nullptr)
    {
        result = BR_BAD_HANDLE;
        goto cleanup_and_return;
    }

    SpaceResult map_result = space_map(space, mem_obj, args->offset, args->size, args->vaddr);

    if (!map_result.succ)
    {
        result = map_result.err;
        goto cleanup_and_return;
    }

    args->vaddr = UNWRAP(map_result).base;
    args->size = UNWRAP(map_result).size;

cleanup_and_return:
    if (space)
    {
        space_deref(space);
    }

    if (mem_obj)
    {
        mem_obj_deref(mem_obj);
    }

    return result;
}

BrResult sys_unmap(BrUnmapArgs *args)
{
    Space *space = nullptr;
    BrResult result = BR_SUCCESS;

    if (args->space == BR_SPACE_SELF)
    {
        space_ref(task_self()->space);
        space = task_self()->space;
    }
    else
    {
        space = (Space *)domain_lookup(task_self()->domain, args->space, BR_OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        result = BR_BAD_HANDLE;
        goto cleanup_and_return;
    }

    SpaceResult unmap_result = space_unmap(space, (VmmRange){args->vaddr, args->size});

    if (!unmap_result.succ)
    {
        result = unmap_result.err;
        goto cleanup_and_return;
    }

cleanup_and_return:
    space_deref(space);

    return result;
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
        space = (Space *)domain_lookup(task_self()->domain, args->space, BR_OBJECT_SPACE);
    }

    if (space == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    Task *task = UNWRAP(task_create(
        str$(&args->name),
        space,
        args->caps & task_self()->caps,
        args->flags | BR_TASK_USER));

    domain_add(task_self()->domain, base$(task));

    space_deref(space);

    *handle = task->handle;

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_create_mem_obj(BrMemObj *handle, BrCreateMemObjArgs *args)
{
    MemObj *mem_obj = nullptr;

    if (args->flags & BR_MEM_OBJ_PMM)
    {
        if (!(task_self()->caps & BR_CAP_PMM))
        {
            return BR_BAD_CAPABILITY;
        }

        mem_obj = mem_obj_pmm((PmmRange){args->addr, args->size}, MEM_OBJ_NONE);
    }
    else
    {
        PmmResult pmm_result = pmm_alloc(args->size);

        if (!pmm_result.succ)
        {
            return pmm_result.err;
        }

        mem_obj = mem_obj_pmm(UNWRAP(pmm_result), MEM_OBJ_OWNING);
    }

    domain_add(task_self()->domain, base$(mem_obj));
    *handle = mem_obj->handle;
    mem_obj_deref(mem_obj);

    return BR_SUCCESS;
}

BrResult sys_create_space(BrSpace *handle, BrCreateSpaceArgs *args)
{
    Space *space = space_create(args->flags);

    domain_add(task_self()->domain, base$(space));
    *handle = space->handle;
    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_create_irq(BrIrq *handle, BrCreateIrqArgs *args)
{
    Irq *irq = irq_create(args->irq);

    domain_add(task_self()->domain, base$(irq));
    *handle = irq->handle;

    irq_deref(irq);

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
        return sys_create_task(&args->handle, &args->task);

    case BR_OBJECT_SPACE:
        return sys_create_space(&args->handle, &args->space);

    case BR_OBJECT_MEMORY:
        return sys_create_mem_obj(&args->handle, &args->mem_obj);

    case BR_OBJECT_IRQ:
        if (!(task_self()->caps & BR_CAP_IRQ))
        {
            return BR_BAD_CAPABILITY;
        }
        return sys_create_irq(&args->handle, &args->irq);

    default:
        return BR_BAD_ARGUMENTS;
    }
}

BrResult sys_start(BrStartArgs *args)
{
    Task *task = (Task *)domain_lookup(task_self()->domain, args->task, BR_OBJECT_TASK);

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    sched_start(task, args->ip, args->sp, args->args);

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
        task = (Task *)domain_lookup(task_self()->domain, args->task, BR_OBJECT_TASK);
    }

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    sched_stop(task, args->exit_value);

    task_deref(task);

    return BR_SUCCESS;
}

static BrResult sys_ipc_send(BrTask to, BrMsg *msg, BrDeadline deadline, BrIpcFlags flags)
{
    Task *task CLEANUP(object_cleanup) = nullptr;
    Envelope envelope CLEANUP(envelope_cleanup) = {};

    msg->from = task_self()->handle;
    task = (Task *)global_lookup(to, BR_OBJECT_TASK);

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    BrResult result = envelope_send(&envelope, msg, task_self()->domain);

    if (result != BR_SUCCESS)
    {
        return result;
    }

    return channel_send(task->channel, &envelope, deadline, flags);
}

static BrResult sys_ipc_recv(BrMsg *msg, BrDeadline deadline, BrIpcFlags flags)
{
    Envelope envelope CLEANUP(envelope_cleanup) = {};

    BrResult result = channel_recv(task_self()->channel, &envelope, deadline, flags);

    if (result != BR_SUCCESS)
    {
        return result;
    }

    envelope_recv(&envelope, msg, task_self()->domain);

    return result;
}

BrResult sys_ipc(BrIpcArgs *args)
{
    BrResult result = BR_SUCCESS;

    if (args->flags & BR_IPC_SEND)
    {
        result = sys_ipc_send(args->to, &args->msg, args->deadline, args->flags);
    }

    if (result != BR_SUCCESS)
    {
        return result;
    }

    if (args->flags & BR_IPC_RECV)
    {
        result = sys_ipc_recv(&args->msg, args->deadline, args->flags);
    }

    return result;
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
        task = (Task *)domain_lookup(task_self()->domain, args->task, BR_OBJECT_TASK);
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

BrResult sys_bind(BrBindArgs *args)
{
    if (!(task_self()->caps & BR_CAP_IRQ))
    {
        return BR_BAD_CAPABILITY;
    }

    Task *task = nullptr;
    task_ref(task_self());
    task = task_self();

    Irq *irq = (Irq *)domain_lookup(task->domain, args->handle, BR_OBJECT_IRQ);

    if (!irq)
    {
        return BR_BAD_HANDLE;
    }

    return irq_bind(task->handle, args->flags, irq);
}

BrResult sys_unbind(BrUnbindArgs *args)
{
    if (!(task_self()->caps & BR_CAP_IRQ))
    {
        return BR_BAD_CAPABILITY;
    }

    Task *task = nullptr;

    task_ref(task_self());
    task = task_self();

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    Irq *irq = (Irq *)domain_lookup(task->domain, args->handle, BR_OBJECT_IRQ);

    if (!irq)
    {
        return BR_BAD_HANDLE;
    }

    return irq_unbind(args->flags, irq);
}

BrResult sys_ack(BrAckArgs *args)
{
    if (!(task_self()->caps & BR_CAP_IRQ))
    {
        return BR_BAD_CAPABILITY;
    }

    Task *task = nullptr;

    task_ref(task_self());
    task = task_self();

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    Irq *irq = (Irq *)domain_lookup(task->domain, args->handle, BR_OBJECT_IRQ);

    if (!irq)
    {
        return BR_BAD_HANDLE;
    }

    return irq_ack(irq);
}

typedef BrResult BrSyscallFn();

BrSyscallFn *syscalls[BR_SYSCALL_COUNT] = {
    [BR_SC_LOG] = sys_log,
    [BR_SC_MAP] = sys_map,
    [BR_SC_UNMAP] = sys_unmap,
    [BR_SC_CREATE] = sys_create,
    [BR_SC_START] = sys_start,
    [BR_SC_EXIT] = sys_exit,
    [BR_SC_IPC] = sys_ipc,
    [BR_SC_DROP] = sys_drop,
    [BR_SC_CLOSE] = sys_close,
    [BR_SC_BIND] = sys_bind,
    [BR_SC_UNBIND] = sys_unbind,
    [BR_SC_ACK] = sys_ack,
};

BrResult syscall_dispatch(BrSyscall syscall, BrArg args)
{
    if (syscall >= BR_SYSCALL_COUNT)
    {
        return BR_BAD_SYSCALL;
    }

    task_begin_syscall();

    BrResult result = syscalls[syscall](args);

    if (result != BR_SUCCESS)
    {
        log$("Syscall: {}({}): {}({#p}) -> {}",
             str$(&task_self()->name),
             task_self()->handle,
             str$(br_syscall_to_string(syscall)),
             args,
             str$(br_result_to_string(result)));
    }

    task_end_syscall();

    return result;
}
