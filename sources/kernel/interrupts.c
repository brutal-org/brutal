
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include <syscalls/types.h>
#include "kernel/interrupts.h"
#include "kernel/task.h"

static IrqBinding *interrupts = nullptr;
static size_t interrupt_count = 0;
static Lock lock = {};

static IrqBinding *irq_binding_get(Irq id)
{
    IrqBinding *binding = interrupts;
    size_t i = 0;
    while (i < interrupt_count)
    {

        if (binding->irq->handle == id.handle)
        {
            return binding;
        }
        binding = binding->next;
        i++;
    }
    return nullptr;
}

static void interrupt_binding_destroy(IrqBinding *handler)
{
    IrqBinding *save = handler;

    handler->next->prev = handler->prev;
    handler->prev->next = handler->next;

    alloc_free(alloc_global(), save);

    interrupt_count--;
    // handle the case were all interrupts are being deleted
    if (interrupt_count == 0)
    {
        interrupts = nullptr;
    }
}

static void irq_destroy(Irq *handler)
{
    auto binding = irq_binding_get(*handler);
    if (binding)
    {
        interrupt_binding_destroy(binding);
    }
}

static IrqBinding *irq_alloc()
{
    interrupt_count++;

    IrqBinding *self = alloc_make(alloc_global(), IrqBinding);
    if (interrupts == nullptr)
    {
        self->prev = self;
        self->next = self;
        interrupts = self;
    }
    else
    {
        self->next = interrupts;
        self->prev = interrupts->prev;
        interrupts->prev->next = self;
        interrupts->prev = self;
    }

    return self;
}

static BrResult send_irq_handled(IrqBinding *self)
{
    BrMsg msg = self->message;
    BrTask to = self->target_task->handle;
    Task *task CLEANUP(object_cleanup) = nullptr;

    Envelope envelope CLEANUP(envelope_cleanup) = {};

    msg.from = self->from->handle;
    task = (Task *)global_lookup(to, BR_OBJECT_TASK);

    if (!task)
    {
        log_unlock("error: can't send interrupt message (invalid task)");
        return BR_BAD_HANDLE;
    }

    BrResult result = envelope_send(&envelope, &msg, self->from->domain);

    if (result != BR_SUCCESS)
    {
        log_unlock("error: can't send interrupt message (invalid message)");
        return result;
    }

    return channel_send(task->channel, &envelope, 0, BR_IPC_SEND);
}

BrResult irq_bindings_destroy_task(const Task *from)
{
    IrqBinding *binding = interrupts;

    while (binding != NULL)
    {
        if (binding->from == from)
        {
            interrupt_binding_destroy(binding);
        }
    }
    return BR_SUCCESS;
}

BrResult irq_handler_bind(Task *task, BrIrqFlags flags, Irq *irq, BrMsg *message)
{
    LOCK_RETAINER(&lock);
    log(" binding interrupt {}", irq->irq);
    auto binding = irq_binding_get(*irq);
    if (binding == nullptr)
    {
        binding = irq_alloc();
    }
    irq_handler_ref(irq);
    binding->target_task = task;
    binding->from = task_self();
    binding->flags = flags;
    binding->message = *message;
    binding->ack = true;
    binding->irq = irq;
    return BR_SUCCESS;
}

BrResult irq_handler_unbind(BrIrqFlags flags, Irq *irq)
{
    LOCK_RETAINER(&lock);
    log(" unbinding interrupt {}", irq->irq);

    auto binding = irq_binding_get(*irq);
    if (binding == nullptr)
    {
        log(" already unbinded interrupt {}", irq->irq);
        return BR_BAD_HANDLE;
    }
    binding->flags = flags;
    interrupt_binding_destroy(binding);
    irq_handler_deref(irq);
    return BR_SUCCESS;
}

BrResult irq_handler_ack(Irq *irq)
{
    LOCK_RETAINER(&lock);
    auto binding = irq_binding_get(*irq);

    if (binding == nullptr)
    {
        return BR_BAD_HANDLE;
    }

    binding->ack = true;
    return BR_SUCCESS;
}

void irq_handler_update(BrIrq interrupt)
{
    LOCK_RETAINER(&lock);
    IrqBinding *binding = interrupts;
    size_t i = 0;
    while (i < interrupt_count)
    {
        if (binding->irq->irq == interrupt && binding->ack)
        {
            send_irq_handled(binding);

            if (binding->flags & BR_IRQ_BIND_ONCE)
            {
                irq_handler_unbind(binding->flags, binding->irq);
            }

            binding->ack = false;
        }

        binding = binding->next;
        i++;
    }
}

Irq *irq_create(BrIrqId id)
{
    auto intt = alloc_make(alloc_global(), Irq);
    intt->irq = id;
    object_init(base$(intt), BR_OBJECT_IRQ, (ObjectDtor *)irq_destroy);
    return intt;
}

void irq_handler_ref(Irq *self)
{
    object_ref(base$(self));
}
void irq_handler_deref(Irq *self)
{
    object_deref(base$(self));
}
