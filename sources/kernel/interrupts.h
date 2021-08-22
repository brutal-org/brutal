#pragma once

#include <brutal/types.h>
#include "kernel/domain.h"
#include "kernel/task.h"

typedef struct
{
    OBJECT_HEADER;
    BrIrq irq;
} Irq;

typedef struct irq_binding IrqBinding;

typedef struct irq_binding
{
    Irq *irq;
    BrIrqFlags flags;
    BrTask target_task;
    BrMsg message;
    bool ack;
} IrqBinding;

BrResult irq_bindings_destroy_task(const Task *target_task);

BrResult irq_handler_bind(BrTask task, BrIrqFlags flags, Irq *irq, BrMsg *message);

BrResult irq_handler_unbind(BrIrqFlags flags, Irq *irq);

BrResult irq_handler_ack(Irq *irq);

void irq_handler_update(BrIrq interrupt_id); // called by the arch

Irq *irq_create(BrIrqId interrupt_id);

void irq_handler_ref(Irq *self);

void irq_handler_deref(Irq *self);
