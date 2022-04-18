#include <bal/task.h>
#include <elf/elf.h>
#include <json/parser.h>
#include <brutal-debug>
#include "system/unit.h"

void unit_init(Unit *self, Str name, BalMem payload, Alloc *alloc)
{
    self->state = UNIT_PENDING;
    self->name = name;
    self->ready = 0;
    self->payload = payload;
    vec_init(&self->consume, alloc);
}

void unit_init_from_module(Unit *self, HandoverModule mod, Alloc *alloc)
{
    BalMem module_mem = {};
    bal_mem_init_pmm(&module_mem, mod.addr, mod.size);
    bal_mem_map(&module_mem);

    Str manifest_str = elf_manifest((Elf64Header *)module_mem.buf);

    Json manifest_json = json_parse_str(manifest_str, alloc);

    Str unit_name = json_get(manifest_json, str$("name")).string;

    unit_init(self, unit_name, module_mem, alloc);

    Json consume = json_get(manifest_json, str$("consume"));

    for (int j = 0; j < json_len(consume); j++)
    {
        IpcProto proto = json_at(consume, j).number;
        unit_consume(self, proto);
    }

    Json rights = json_get(manifest_json, str$("rights"));

    for (int j = 0; j < json_len(rights); j++)
    {
        BrRight right = br_right_from_str(json_at(rights, j).string);

        if (right == 0)
        {
            log$("Invalid right: {}", json_at(rights, j).string);
        }

        self->rights |= right;
    }
}

void unit_deinit(Unit *self)
{
    vec_deinit(&self->consume);
}

void unit_consume(Unit *self, IpcProto proto)
{
    log$("Unit '{case-pascal}' is consuming capability '{x}'...", self->name, proto);

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
            log$("Providing service '{case-pascal}' with capability '{x}'...", self->name, new_cap.proto);

            *cap = new_cap;
            self->ready++;

            if (self->ready == self->consume.len)
            {
                log$("Service '{case-pascal}' ready!", self->name);
                self->state = UNIT_READY;
            }
            return;
        }
    }
}

void unit_start(Unit *self)
{
    if (self->state != UNIT_READY)
        return;

    log$("Starting service '{case-pascal}'...", self->name);

    BalTask elf_task;
    bal_task_init(&elf_task, self->name);

    bal_TaskExec(
        &elf_task,
        &self->payload,
        self->rights,
        self->consume.data,
        self->consume.len);

    log$("Service '{case-pascal}' with id={} started!", self->name, elf_task.id);

    self->state = UNIT_RUNNING;
}
