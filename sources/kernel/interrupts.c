
#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include <syscalls/types.h>
#include "kernel/interrupts.h"

static Vec(IrqHandler *) interrupts = {};

bool loaded = false;

void interrupt_routing_initialize()
{
    log("Initializing interrupt routing...");
    vec_init(&interrupts, alloc_global());
}
void interrupt_destroy(IrqHandler *handler)
{
    log(" destroying interrupt {}", handler->irq);
    alloc_free(alloc_global(), handler);
};

BrResult irq_handler_bind(Task *task, BrIrqFlags flags, IrqHandler *irq, BrMsg *message)
{
    log(" binding interrupt {}", irq->irq);
    irq_handler_ref(irq);
    irq->target_task = task;
    irq->from_task = task_self();
    irq->flags = flags;
    irq->message = *message;
    vec_push(&interrupts, irq);
    return BR_SUCCESS;
}

BrResult irq_handler_unbind(BrIrqFlags flags, IrqHandler *irq)
{
    log(" unbinding interrupt {}", irq->irq);
    irq->target_task = nullptr;
    irq->flags = flags;
    irq_handler_deref(irq);
    vec_remove(&interrupts, irq);
    return BR_SUCCESS;
}

BrResult irq_handler_ack(IrqHandler *irq)
{
    irq->ack = true;
    return BR_SUCCESS;
}

BrResult send_irq_handled(IrqHandler *self)
{
    BrMsg msg = self->message;
    BrTask to = self->target_task->handle;
    Task *task CLEANUP(object_cleanup) = nullptr;

    Envelope envelope CLEANUP(envelope_cleanup) = {};

    msg.from = self->from_task->handle;
    task = (Task *)global_lookup(to, BR_OBJECT_TASK);

    if (!task)
    {
        log_unlock("error: can't send interrupt message (invalid task)");
        return BR_BAD_HANDLE;
    }

    BrResult result = envelope_send(&envelope, &msg, task_self()->domain);

    if (result != BR_SUCCESS)
    {
        log_unlock("error: can't send interrupt message (invalid message)");
        return result;
    }

    return channel_send(task->channel, &envelope, 0, BR_IPC_SEND);
}

void irq_handler_update(BrIrq interrupt)
{
    vec_foreach(interr, &interrupts)
    {
        if (interr->irq == interrupt && interr->ack)
        {
            send_irq_handled(interr);

            if (interr->flags & BR_IRQ_BIND_ONCE)
            {
                irq_handler_unbind(interr->flags, interr);
            }

            interr->ack = false;
        }
    }
}

IrqHandler *irq_handler_create(BrIrqId id)
{
    auto intt = alloc_make(alloc_global(), IrqHandler);
    intt->flags = BR_IRQ_BIND;
    intt->irq = id;
    intt->ack = true;
    object_init(base$(intt), BR_OBJECT_IRQ, (ObjectDtor *)interrupt_destroy);
    return intt;
}

void irq_handler_ref(IrqHandler *self)
{
    object_ref(base$(self));
}
void irq_handler_deref(IrqHandler *self)
{
    object_deref(base$(self));
}
