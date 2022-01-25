#include <bal/task.h>
#include <brutal/debug.h>
#include "init/unit.h"

void unit_init(Unit *self, Str name, Alloc *alloc)
{
    self->state = UNIT_PENDING;
    self->name = name;
    self->ready = 0;
    vec_init(&self->consume, alloc);
}

void unit_deinit(Unit *self)
{
    vec_deinit(&self->consume);
}

void unit_consume(Unit *self, IpcProto proto)
{
    log$("Unit '{case:pascal}' is consuming capability '{}'...", self->name, proto);

    IpcCap cap = {};
    cap.proto = proto;
    vec_push(&self->consume, cap);
}

void unit_provide(Unit *self, IpcCap new_cap)
{
    vec_foreach(cap, &self->consume)
    {
        if (cap->proto == new_cap.proto)
        {
            log$("Providing service '{case:pascal}' with capability '{}'...", self->name, new_cap.proto);

            *cap = new_cap;
            self->ready++;

            if (self->ready == self->consume.len)
            {
                log$("Service '{case:pascal}' ready!", self->name);
                self->state = UNIT_READY;
            }
            return;
        }
    }
}

void unit_start(Unit *self, Handover *handover)
{
    if (self->state != UNIT_READY)
        return;

    log$("Starting service '{case:pascal}'...", self->name);

    HandoverModule const *elf = handover_find_module(handover, self->name);
    assert_not_null(elf);

    BalMem elf_mem;
    bal_mem_init_pmm(&elf_mem, elf->addr, elf->size);

    BalTask elf_task;
    bal_task_init(&elf_task, self->name);

    bal_task_exec(
        &elf_task,
        &elf_mem,
        BR_RIGHT_IO | BR_RIGHT_LOG | BR_RIGHT_PMM | BR_RIGHT_IRQ,
        self->consume.data,
        self->consume.len);

    bal_mem_deinit(&elf_mem);

    log$("Service '{case:pascal}' started!", self->name);

    self->state = UNIT_RUNNING;
}
