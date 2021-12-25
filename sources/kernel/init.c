#include <brutal/alloc.h>
#include <brutal/debug.h>
#include "kernel/heap.h"
#include "kernel/init.h"
#include "kernel/kernel.h"
#include "kernel/mmap.h"
#include "kernel/sched.h"

static Task *_task = nullptr;

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

    if (header->ident.elf_class != ELF_CLASS_64)
    {
        log$("not 64bit ");
        return false;
    }

    if (header->ident.data_encoding != ELF_ENCODING_LITTLE_ENDIAN)
    {
        log$("not little endian");
        return false;
    }

    // don't start non executable files
    return true;
}

static void elf_load_program(Task *task, Elf64Header const *elf_header, Memory *elf_obj)
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

            Memory *prog_obj = memory_heap(heap, MEMORY_OWNING);
            UNWRAP(space_map(task->space, prog_obj, 0, 0, prog_header->virtual_address));
            memory_deref(prog_obj);
        }

        prog_header = (Elf64ProgramHeader *)((uint8_t *)prog_header + elf_header->program_header_table_entry_size);
    }
}

void init_stack(Task *task)
{
    HeapRange heap = UNWRAP(heap_alloc(KERNEL_STACK_SIZE));
    Memory *obj = memory_heap(heap, MEMORY_OWNING);
    space_map(task->space, obj, 0, 0, USER_STACK_BASE - KERNEL_STACK_SIZE);
    memory_deref(obj);
}

static uintptr_t init_pass(Task *task, Handover const *handover)
{
    HeapRange heap = UNWRAP(heap_alloc(ALIGN_UP(sizeof(Handover), MEM_PAGE_SIZE)));
    mem_cpy((void *)heap.base, handover, sizeof(Handover));
    Memory *obj = memory_heap(heap, MEMORY_OWNING);
    uintptr_t addr = UNWRAP(space_map(task->space, obj, 0, 0, 0)).base;
    memory_deref(obj);

    return addr;
}

void init_start(Handover const *handover)
{
    Str name = str$("init");

    HandoverModule const *elf_module = handover_find_module(handover, name);

    if (elf_module == nullptr)
    {
        panic$("can't find 'init' handover module !");
    }

    Elf64Header *elf_header = (Elf64Header *)mmap_phys_to_io(elf_module->addr);
    Memory *elf_obj = memory_pmm((HeapRange){elf_module->addr, elf_module->size}, MEMORY_NONE);

    assert_truth(elf_supported(elf_header, elf_module->size));

    Space *space = space_create(0);
    Task *task = UNWRAP(task_create(space, BR_RIGHT_IRQ | BR_RIGHT_PMM | BR_RIGHT_LOG | BR_RIGHT_TASK | BR_RIGHT_IO, BR_TASK_USER));
    space_deref(space);

    elf_load_program(task, elf_header, elf_obj);

    init_stack(task);
    uintptr_t hoaddr = init_pass(task, handover);

    task_ref(task);
    _task = task;

    sched_start(
        task,
        elf_header->entry,
        USER_STACK_BASE,
        (BrTaskArgs){
            .type = BR_START_HANDOVER,
            .arg1 = hoaddr,
        });

    task_deref(task);
    memory_deref(elf_obj);
}

Task *init_task(void)
{
    assert_not_null(_task);
    task_ref(_task);
    return _task;
}
