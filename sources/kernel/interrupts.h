#pragma once

#include <brutal/types.h>
#include "kernel/domain.h"
#include "kernel/task.h"

typedef struct
{
    OBJECT_HEADER;
    BrIrq irq;
} Irq;

typedef struct irq_binding
{
    Irq *irq;
    BrIrqFlags flags;
    BrTask target_task;
    bool ack;
} IrqBinding;

BrResult irq_unbind_all(const Task *target_task);

BrResult irq_bind(BrTask task, BrIrqFlags flags, Irq *irq);

BrResult irq_unbind(BrIrqFlags flags, Irq *irq);

BrResult irq_ack(Irq *irq);

void irq_dispatch(BrIrq interrupt_id); // called by the arch

Irq *irq_create(BrIrqId interrupt_id);

void irq_ref(Irq *self);

void irq_deref(Irq *self);
