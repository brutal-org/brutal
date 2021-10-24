
#include <bal/abi.h>
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include "kernel/interrupts.h"
#include "kernel/task.h"

typedef list(IrqBinding) IrqBindingEntry;
static IrqBindingEntry interrupts = {};
static Lock lock = {};

static IrqBindingEntry *irq_binding_get(Irq *irq)
{
    list_loop(binding, &interrupts)
    {
        if (binding->data->irq == irq)
        {
            return binding;
        }
    }

    return nullptr;
}

static void interrupt_binding_destroy(IrqBindingEntry *handler)
{
    list_remove(&interrupts, handler);
}

static void irq_destroy(Irq *handler)
{
    IrqBindingEntry *binding = irq_binding_get(handler);

    if (binding)
    {
        interrupt_binding_destroy(binding);
    }
}

static IrqBindingEntry *irq_alloc(void)
{

    if (interrupts.alloc == nullptr)
    {
        list_init(&interrupts, alloc_global());
    }

    IrqBindingEntry *res = list_insert(&interrupts, (IrqBinding){});

    return res;
}

static BrResult send_irq_handled(IrqBinding *self)
{
    return channel_send(
        self->task->channel,
        nullptr,
        &(BrMsg){
            .from = BR_TASK_IRQ,
            .arg[0] = self->irq->irq,
        },
        0, BR_IPC_SEND);
}

BrResult irq_unbind_all(Task const *task)
{
    list_loop(binding, &interrupts)
    {
        if (binding->data->task == task)
        {
            interrupt_binding_destroy(binding);
        }
    }

    return BR_SUCCESS;
}

BrResult irq_bind(Task *task, BrIrqFlags flags, Irq *irq)
{
    LOCK_RETAINER(&lock);
    log$("Binding interrupt {}", irq->irq);

    IrqBindingEntry *binding = irq_binding_get(irq);

    if (binding == nullptr)
    {
        binding = irq_alloc();
    }

    irq_ref(irq);

    binding->data->task = task;
    binding->data->flags = flags;
    binding->data->ack = true;
    binding->data->irq = irq;

    return BR_SUCCESS;
}

BrResult irq_unbind(BrIrqFlags flags, Irq *irq)
{
    LOCK_RETAINER(&lock);
    log$("Unbinding interrupt {}", irq->irq);

    IrqBindingEntry *binding = irq_binding_get(irq);

    if (binding == nullptr)
    {
        log$("Already unbinded interrupt {}", irq->irq);
        return BR_BAD_HANDLE;
    }

    binding->data->flags = flags;

    interrupt_binding_destroy(binding);
    irq_deref(irq);
    return BR_SUCCESS;
}

BrResult irq_ack(Irq *irq)
{
    LOCK_RETAINER(&lock);

    IrqBindingEntry *binding = irq_binding_get(irq);

    if (binding == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    binding->data->ack = true;
    return BR_SUCCESS;
}

void irq_dispatch(BrIrq interrupt)
{
    LOCK_RETAINER(&lock);

    list_loop(binding, &interrupts)
    {
        if (binding->data->irq->irq == interrupt && binding->data->ack)
        {
            send_irq_handled(binding->data);

            binding->data->ack = false;

            if (binding->data->flags & BR_IRQ_BIND_ONCE)
            {
                irq_unbind(binding->data->flags, binding->data->irq);
            }
        }
    }
}

Irq *irq_create(BrIrqId id)
{
    Irq *self = alloc_make(alloc_global(), Irq);
    self->irq = id;

    object_init(base$(self), BR_OBJECT_IRQ, (ObjectDtor *)irq_destroy);
    return self;
}

void irq_ref(Irq *self)
{
    object_ref(base$(self));
}

void irq_deref(Irq *self)
{
    object_deref(base$(self));
}
