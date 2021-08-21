#pragma once

#include <brutal/types.h>
#include "kernel/domain.h"
#include "kernel/task.h"

typedef struct
{
    OBJECT_HEADER;
    BrIrq irq;
    BrIrqFlags flags;
    Task *target_task;
    Task *from_task;
    BrMsg message;
    bool ack;
} IrqHandler;

BrResult irq_handler_bind(Task *task, BrIrqFlags flags, IrqHandler *irq, BrMsg *message);
BrResult irq_handler_unbind(BrIrqFlags flags, IrqHandler *irq);
BrResult irq_handler_ack(IrqHandler *irq);

void irq_handler_update(BrIrq interrupt_id); // called by the arch

IrqHandler *irq_handler_create(BrIrqId interrupt_id);

void irq_handler_ref(IrqHandler *self);

void irq_handler_deref(IrqHandler *self);

void interrupt_routing_initialize();
