
#include <brutal/alloc.h>
#include <brutal/log.h>
#include "kernel/heap.h"
#include "kernel/init.h"
#include "kernel/kernel.h"

static bool elf_supported(Elf64Header const *header, size_t data_size)
{
    if (data_size < sizeof(Elf64Header))
    {
        log("not valid elf size");
        return false;
    }

    if (!elf_validate(header))
    {
        log("not valid elf header");
    }

    if (header->identifier.elf_class != ELF_CLASS_64)
    {
        log("not 64bit ");
        return false;
    }

    if (header->identifier.data_encoding != ELF_ENCODING_LITTLE_ENDIAN)
    {
        log("not little endian");
        return false;
    }

    // don't start non executable files
    return true;
}

static void elf_load_program(Task *task, Elf64Header const *elf, void const *data)
{
    Elf64ProgramHeader *header = (Elf64ProgramHeader *)(data + elf->program_header_table_file_offset);

    for (size_t i = 0; i < elf->program_header_table_entry_count; i++)
    {
        if (header->type != ELF_PROGRAM_HEADER_LOAD)
        {
            log("Ignoring segment with type={#0x}", header->type);
            continue;
        }

        auto size = MAX(header->memory_size, header->file_size);
        auto range = UNWRAP(heap_alloc(size));
        auto ptr = (uint8_t *)range.base;

        mem_set(ptr, 0, size);
        mem_cpy(ptr, data + header->file_offset, header->file_size);

        VmmRange target = (VmmRange){
            .base = header->virtual_address,
            .size = ALIGN_UP(header->memory_size, HOST_MEM_PAGESIZE),
        };

        UNWRAP(space_map_pmm(task->space, target, heap_to_pmm(range)));

        header = (Elf64ProgramHeader *)((uintptr_t)header + elf->program_header_table_entry_size);
    }
}

static BrTask init = BR_TASK_ERROR;

void init_start(Handover *handover)
{
    auto name = str_cast("init");
    auto module = handover_find_module(handover, name);
    auto elf = (Elf64Header *)module->addr;

    assert_truth(elf_supported(elf, module->size));

    auto space = space_create();
    Task *task = UNWRAP(task_create(name, space, BR_TASK_USER));
    space_deref(space);

    elf_load_program(task, elf, (void const *)module->addr);

    // Create the user stack.
    space_map(task->space, (VmmRange){USER_STACK_BASE - KERNEL_STACK_SIZE, KERNEL_STACK_SIZE});

    init = task->base.handle;
    task_start(task, elf->entry, USER_STACK_BASE, (BrTaskArgs){});
}

BrTask init_handle(void)
{
    assert_not_equal(init, BR_TASK_ERROR);
    return init;
}
