
#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/heap.h"
#include "kernel/init.h"
#include "kernel/kernel.h"
#include "kernel/mmap.h"
#include "kernel/sched.h"

static bool elf_supported(Elf64Header const *header, size_t data_size)
{
    if (data_size < sizeof(Elf64Header))
    {
        log$("not valid elf size");
        return false;
    }

    if (!elf_validate(header))
    {
        log$("not valid elf header");
    }

    if (header->identifier.elf_class != ELF_CLASS_64)
    {
        log$("not 64bit ");
        return false;
    }

    if (header->identifier.data_encoding != ELF_ENCODING_LITTLE_ENDIAN)
    {
        log$("not little endian");
        return false;
    }

    // don't start non executable files
    return true;
}

static void elf_load_program(Task *task, Elf64Header const *elf_header, MemObj *elf_obj)
{
    Elf64ProgramHeader *prog_header = (Elf64ProgramHeader *)((uint8_t *)elf_header + elf_header->program_header_table_file_offset);

    for (size_t i = 0; i < elf_header->program_header_table_entry_count; i++)
    {
        if (prog_header->type != ELF_PROGRAM_HEADER_LOAD)
        {
            log$("Ignoring segment with type={#0x}", prog_header->type);
            continue;
        }

        size_t size = ALIGN_UP(MAX(prog_header->memory_size, prog_header->file_size), MEM_PAGE_SIZE);

        if (!(prog_header->flags & ELF_PROGRAM_HEADER_WRITABLE) &&
            prog_header->file_size == prog_header->memory_size)
        {
            UNWRAP(space_map(task->space, elf_obj, prog_header->file_offset, size, prog_header->virtual_address));
        }
        else
        {
            HeapRange heap = UNWRAP(heap_alloc(size));
            uint8_t *ptr = (uint8_t *)heap.base;

            mem_set(ptr, 0, size);
            mem_cpy(ptr, (uint8_t *)elf_header + prog_header->file_offset, prog_header->file_size);

            MemObj *prog_obj = mem_obj_heap(heap, MEM_OBJ_OWNING);
            UNWRAP(space_map(task->space, prog_obj, 0, 0, prog_header->virtual_address));
            mem_obj_deref(prog_obj);
        }

        prog_header = (Elf64ProgramHeader *)((uint8_t *)prog_header + elf_header->program_header_table_entry_size);
    }
}

static Task *init_task = nullptr;

void init_stack(Task *task)
{
    HeapRange heap = UNWRAP(heap_alloc(KERNEL_STACK_SIZE));
    MemObj *obj = mem_obj_heap(heap, MEM_OBJ_OWNING);
    space_map(task->space, obj, 0, 0, USER_STACK_BASE - KERNEL_STACK_SIZE);
    mem_obj_deref(obj);
}

static uintptr_t init_pass(Task *task, Handover const *handover)
{
    HeapRange heap = UNWRAP(heap_alloc(ALIGN_UP(sizeof(Handover), MEM_PAGE_SIZE)));
    mem_cpy((void *)heap.base, handover, sizeof(Handover));
    MemObj *obj = mem_obj_heap(heap, MEM_OBJ_OWNING);
    uintptr_t addr = UNWRAP(space_map(task->space, obj, 0, 0, 0)).base;
    mem_obj_deref(obj);

    return addr;
}

void init_start(Handover const *handover)
{
    Str name = str$("init");

    HandoverModule const *elf_module = handover_find_module(handover, name);
    Elf64Header *elf_header = (Elf64Header *)mmap_phys_to_io(elf_module->addr);
    MemObj *elf_obj = mem_obj_pmm((HeapRange){elf_module->addr, elf_module->size}, MEM_OBJ_NONE);

    assert_truth(elf_supported(elf_header, elf_module->size));

    Space *space = space_create(BR_SPACE_NONE);
    Task *task = UNWRAP(task_create(name, space, BR_CAP_ALL, BR_TASK_USER));
    space_deref(space);

    elf_load_program(task, elf_header, elf_obj);

    init_stack(task);
    uintptr_t hoaddr = init_pass(task, handover);

    task_ref(task);
    init_task = task;

    sched_start(task, elf_header->entry, USER_STACK_BASE, (BrTaskArgs){.arg1 = hoaddr});

    task_deref(task);
    mem_obj_deref(elf_obj);
}

Task *init_get_task(void)
{
    assert_not_null(init_task);
    task_ref(init_task);
    return init_task;
}
