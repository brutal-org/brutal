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
    Task *target_task;
    Task *from;
    BrMsg message;
    bool ack;
    IrqBinding *next;
    IrqBinding *prev;
} IrqBinding;

BrResult irq_bindings_destroy_task(const Task *from);

BrResult irq_handler_bind(Task *task, BrIrqFlags flags, Irq *irq, BrMsg *message);
BrResult irq_handler_unbind(BrIrqFlags flags, Irq *irq);
BrResult irq_handler_ack(Irq *irq);

void irq_handler_update(BrIrq interrupt_id); // called by the arch

Irq *irq_create(BrIrqId interrupt_id);

void irq_handler_ref(Irq *self);

void irq_handler_deref(Irq *self);
