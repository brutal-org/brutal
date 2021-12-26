#include <bal/abi.h>
#include <brutal/debug.h>
#include <embed/log.h>
#include "kernel/arch.h"
#include "kernel/domain.h"
#include "kernel/event.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "kernel/sched.h"
#include "kernel/syscalls.h"
#include "kernel/task.h"

BrResult sys_log(BrLogArgs *args)
{
    if (!(task_self()->rights & BR_RIGHT_LOG))
    {
        return BR_NOT_PERMITTED;
    }

    embed_log_lock();
    io_write(embed_log_writer(), (uint8_t *)args->message, args->size);
    embed_log_unlock();

    return BR_SUCCESS;
}

BrResult sys_map(BrMapArgs *args)
{
    Space *space = nullptr;

    if (args->space == BR_HANDLE_SELF)
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

    Memory *memory = (Memory *)domain_lookup(task_self()->domain, args->memory, BR_OBJECT_MEMORY);

    if (memory == nullptr)
    {
        space_deref(space);

        return BR_BAD_HANDLE;
    }

    SpaceResult map_result = space_map(space, memory, args->offset, args->size, args->vaddr);

    if (!map_result.succ)
    {
        space_deref(space);
        memory_deref(memory);

        return map_result.err;
    }

    args->vaddr = UNWRAP(map_result).base;
    args->size = UNWRAP(map_result).size;

    return BR_SUCCESS;
}

BrResult sys_unmap(BrUnmapArgs *args)
{
    Space *space = nullptr;

    if (args->space == BR_HANDLE_SELF)
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

    SpaceResult unmap_result = space_unmap(space, (VmmRange){args->vaddr, args->size});

    if (!unmap_result.succ)
    {
        space_deref(space);
        return unmap_result.err;
    }

    return BR_SUCCESS;
}

BrResult sys_create_task(BrId *id, BrHandle *handle, BrTaskProps *args)
{
    Space *space = nullptr;

    if (args->space == BR_HANDLE_SELF)
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
        space,
        args->rights & task_self()->rights,
        args->flags | BR_TASK_USER));

    *id = task->id;
    *handle = domain_add(task_self()->domain, base$(task));

    space_deref(space);

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_create_memory(BrId *id, BrHandle *handle, BrMemoryProps *args)
{
    Memory *memory = nullptr;

    if (args->flags & BR_MEM_PMM)
    {
        if (!(task_self()->rights & BR_RIGHT_PMM))
        {
            return BR_NOT_PERMITTED;
        }

        memory = memory_pmm((PmmRange){args->addr, args->size}, MEMORY_NONE);
    }
    else
    {
        PmmResult pmm_result = pmm_alloc(args->size, !(args->flags & BR_MEM_LOWER));

        if (!pmm_result.succ)
        {
            return pmm_result.err;
        }

        memory = memory_pmm(UNWRAP(pmm_result), MEMORY_OWNING);
        args->addr = UNWRAP(pmm_result).base;
    }

    *id = memory->id;
    *handle = domain_add(task_self()->domain, base$(memory));

    memory_deref(memory);

    return BR_SUCCESS;
}

BrResult sys_create_space(BrId *id, BrHandle *handle, BrSpaceProps *args)
{
    Space *space = space_create(args->flags);

    *id = space->id;
    *handle = domain_add(task_self()->domain, base$(space));

    space_deref(space);

    return BR_SUCCESS;
}

BrResult sys_create(BrCreateArgs *args)
{
    switch (args->type)
    {
    case BR_OBJECT_TASK:
        if (!(task_self()->rights & BR_RIGHT_TASK))
        {
            return BR_NOT_PERMITTED;
        }

        return sys_create_task(&args->id, &args->handle, &args->task);

    case BR_OBJECT_SPACE:
        return sys_create_space(&args->id, &args->handle, &args->space);

    case BR_OBJECT_MEMORY:
        return sys_create_memory(&args->id, &args->handle, &args->memory);

    default:
        return BR_BAD_ARGUMENTS;
    }
}

BrResult sys_start(BrStartArgs *args)
{
    Task *task = (Task *)domain_lookup(task_self()->domain, args->handle, BR_OBJECT_TASK);

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

    if (args->handle == BR_HANDLE_SELF)
    {
        task = task_self();
        task_ref(task);
    }
    else
    {
        task = (Task *)domain_lookup(task_self()->domain, args->handle, BR_OBJECT_TASK);
    }

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    sched_stop(task, args->result);

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_ipc(BrIpcArgs *args)
{
    BrResult result = BR_SUCCESS;

    if (args->flags & BR_IPC_SEND)
    {
        args->msg.from = task_self()->id;

        Task *task CLEANUP(object_cleanup) = nullptr;

        if (args->to == BR_ID_SUPER)
        {
            task = init_task();
        }
        else
        {
            // FIXME: We should use handle instead of id for IPCs.
            task = (Task *)global_lookup(args->to, BR_OBJECT_TASK);
        }

        if (!task)
        {
            return BR_BAD_ID;
        }

        result = channel_send(
            task->channel,
            task_self()->domain,
            &args->msg,
            args->deadline,
            args->flags);
    }

    if (result != BR_SUCCESS)
    {
        return result;
    }

    if (args->flags & BR_IPC_RECV)
    {
        result = channel_recv(
            task_self()->channel,
            task_self()->domain,
            &args->msg,
            args->deadline,
            args->flags);
    }

    return result;
}

BrResult sys_drop(BrDropArgs *args)
{
    Task *task = nullptr;

    if (args->handle == BR_HANDLE_SELF)
    {
        task_ref(task_self());
        task = task_self();
    }
    else
    {
        task = (Task *)domain_lookup(task_self()->domain, args->handle, BR_OBJECT_TASK);
    }

    if (!task)
    {
        return BR_BAD_HANDLE;
    }

    if (!(task->rights & args->rights))
    {
        task_deref(task);
        return BR_NOT_PERMITTED;
    }

    task->rights = task->rights & ~args->rights;

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
    if (args->event.type == BR_EVENT_IRQ && !(task_self()->rights & BR_RIGHT_IRQ))
    {
        return BR_NOT_PERMITTED;
    }

    return event_bind(task_self(), args->event);
}

BrResult sys_unbind(BrUnbindArgs *args)
{
    return event_unbind(task_self(), args->event);
}

BrResult sys_ack(BrAckArgs *args)
{
    return event_ack(task_self(), args->event);
}

static BrResult sys_stat_memory(BrMemoryInfos *args, Memory *obj)
{
    args->range = obj->pmm;
    return BR_SUCCESS;
}

static BrResult sys_stat_domain(BrDomainInfos *args, Domain *domain)
{
    args->domain_object_count = slot_count(&domain->objects);
    return BR_SUCCESS;
}

static BrResult sys_stat_space(BrSpaceInfos *args, Space *space)
{
    args->flags = space->flags;
    return BR_SUCCESS;
}

static BrResult sys_stat_task(BrTaskInfos *args, Task *task)
{
    args->blocked = task->blocked;
    args->started = task->started;
    args->stopped = task->stopped;
    args->rights = task->rights;

    return BR_SUCCESS;
}

BrResult sys_inspect(BrInspectArgs *args)
{
    Object *object = nullptr;

    if (args->handle == BR_HANDLE_SELF)
    {
        object = (Object *)task_self();
    }
    else
    {
        object = domain_lookup(task_self()->domain, args->handle, BR_OBJECT_ANY);
    }

    if (object == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    args->type = object->type;
    args->handle = args->handle;
    args->id = object->id;

    switch (object->type)
    {
    case BR_OBJECT_MEMORY:
        return sys_stat_memory(&args->memory, (Memory *)object);

    case BR_OBJECT_DOMAIN:
        return sys_stat_domain(&args->domain, (Domain *)object);

    case BR_OBJECT_SPACE:
        return sys_stat_space(&args->space, (Space *)object);

    case BR_OBJECT_TASK:
        return sys_stat_task(&args->task, (Task *)object);

    default:
        panic$("Unknow object type {}", object->type);
    }
}

BrResult sys_in(BrIoArgs *args)
{
    if (!(task_self()->rights & BR_RIGHT_IO))
    {
        return BR_NOT_PERMITTED;
    }

    args->data = arch_in(args->port, args->size);

    return BR_SUCCESS;
}

BrResult sys_out(BrIoArgs *args)
{
    if (!(task_self()->rights & BR_RIGHT_IO))
    {
        return BR_NOT_PERMITTED;
    }

    arch_out(args->port, args->size, args->data);

    return BR_SUCCESS;
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
    [BR_SC_INSPECT] = sys_inspect,
    [BR_SC_IN] = sys_in,
    [BR_SC_OUT] = sys_out,
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
        log$("Syscall: Task({}): {}({#p}) -> {}",
             task_self()->id,
             str$(br_syscall_to_string(syscall)),
             args,
             str$(br_result_to_string(result)));
    }

    task_end_syscall();

    return result;
}
