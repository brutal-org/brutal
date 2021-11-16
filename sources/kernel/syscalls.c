#include <bal/abi.h>
#include <brutal/debug.h>
#include <embed/log.h>
#include "kernel/arch.h"
#include "kernel/domain.h"
#include "kernel/event.h"
#include "kernel/global.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "kernel/sched.h"
#include "kernel/syscalls.h"
#include "kernel/task.h"

BrResult sys_log(BrLogArgs *args)
{
    host_log_lock();
    print(host_log_writer(), "cpu{}: {}({}): ", cpu_self_id(), str$(&task_self()->name), task_self()->id);
    io_write(host_log_writer(), (uint8_t *)args->message, args->size);
    host_log_unlock();

    return BR_SUCCESS;
}

BrResult sys_map(BrMapArgs *args)
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

    MemObj *mem_obj = nullptr;

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
        space_deref(space);

        return BR_BAD_HANDLE;
    }

    SpaceResult map_result = space_map(space, mem_obj, args->offset, args->size, args->vaddr);

    if (!map_result.succ)
    {
        space_deref(space);
        mem_obj_deref(mem_obj);

        return map_result.err;
    }

    args->vaddr = UNWRAP(map_result).base;
    args->size = UNWRAP(map_result).size;

    return BR_SUCCESS;
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

BrResult sys_create_task(BrId *id, BrTask *handle, BrCreateTaskArgs *args)
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

    *id = task->id;
    *handle = domain_add(task_self()->domain, base$(task));

    space_deref(space);

    task_deref(task);

    return BR_SUCCESS;
}

BrResult sys_create_mem_obj(BrId *id, BrMemObj *handle, BrCreateMemObjArgs *args)
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
        PmmResult pmm_result = pmm_alloc(args->size, !(args->flags & BR_MEM_OBJ_LOWER));

        if (!pmm_result.succ)
        {
            return pmm_result.err;
        }

        mem_obj = mem_obj_pmm(UNWRAP(pmm_result), MEM_OBJ_OWNING);
        args->addr = UNWRAP(pmm_result).base;
    }

    *id = mem_obj->id;
    *handle = domain_add(task_self()->domain, base$(mem_obj));

    mem_obj_deref(mem_obj);

    return BR_SUCCESS;
}

BrResult sys_create_space(BrId *id, BrSpace *handle, BrCreateSpaceArgs *args)
{
    Space *space = space_create(args->flags);

    *id = space->id;
    *handle = domain_add(task_self()->domain, base$(space));

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
        return sys_create_task(&args->id, &args->handle, &args->task);

    case BR_OBJECT_SPACE:
        return sys_create_space(&args->id, &args->handle, &args->space);

    case BR_OBJECT_MEMORY:
        return sys_create_mem_obj(&args->id, &args->handle, &args->mem_obj);

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

BrResult sys_ipc(BrIpcArgs *args)
{
    BrResult result = BR_SUCCESS;

    if (args->flags & BR_IPC_SEND)
    {
        args->msg.from = task_self()->id;

        Task *task CLEANUP(object_cleanup) = nullptr;

        if (args->to == BR_TASK_INIT)
        {
            task = init_get_task();
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
    if (args->event.type == BR_EVENT_IRQ && !(task_self()->caps & BR_CAP_IRQ))
    {
        return BR_BAD_CAPABILITY;
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

BrResult sys_stat_memobj(BrInfoMemObj *args, MemObj *obj)
{
    args->range = obj->pmm;
    return BR_SUCCESS;
}

BrResult sys_stat_domain(BrInfoDomain *args, Domain *domain)
{
    args->domain_object_count = slot_count(&domain->objects);
    return BR_SUCCESS;
}

BrResult sys_stat_space(BrInfoSpace *args, Space *space)
{
    args->flags = space->flags;
    return BR_SUCCESS;
}

BrResult sys_stat_task(BrInfoTask *args, Task *task)
{
    args->blocked = task->is_blocked;
    args->started = task->is_started;
    args->stopped = task->is_stopped;
    args->caps = task->caps;
    args->name = task->name;
    args->id = task->id;

    return BR_SUCCESS;
}

BrResult sys_stat(BrStatArgs *args)
{
    Object *object = nullptr;

    if (args->handle == BR_TASK_SELF)
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

    args->info.type = object->type;

    switch (object->type)
    {
    case BR_OBJECT_MEMORY:
        return sys_stat_memobj(&args->info.memobj, (MemObj *)object);

    case BR_OBJECT_DOMAIN:
        return sys_stat_domain(&args->info.domainobj, (Domain *)object);

    case BR_OBJECT_SPACE:
        return sys_stat_space(&args->info.spaceobj, (Space *)object);

    case BR_OBJECT_TASK:
        return sys_stat_task(&args->info.taskobj, (Task *)object);

    default:
        panic$("Unknow object type {}", object->type);
    }
}

BrResult sys_in(BrIoArgs *args)
{
    if (!(task_self()->caps & BR_CAP_IO))
    {
        return BR_BAD_CAPABILITY;
    }

    args->data = arch_in(args->port, args->size);

    return BR_SUCCESS;
}

BrResult sys_out(BrIoArgs *args)
{
    if (!(task_self()->caps & BR_CAP_IO))
    {
        return BR_BAD_CAPABILITY;
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
    [BR_SC_STAT] = sys_stat,
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
        log$("Syscall: {}({}): {}({#p}) -> {}",
             str$(&task_self()->name),
             task_self()->id,
             str$(br_syscall_to_string(syscall)),
             args,
             str$(br_result_to_string(result)));
    }

    task_end_syscall();

    return result;
}
