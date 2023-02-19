#include <brutal-alloc>
#include <brutal-debug>

#include "kernel/heap.h"
#include "kernel/init.h"
#include "kernel/kernel.h"
#include "kernel/mmap.h"
#include "kernel/sched.h"

static bool elf_supported(Elf64Header const *header, size_t data_size)
{
    return data_size >= sizeof(Elf64Header) &&
           elf_validate(header) &&
           header->ident.elf_class == ELF_CLASS_64 &&
           header->ident.data_encoding == ELF_ENCODING_LITTLE_ENDIAN;
}

static void elf_load_program(Task *task, Elf64Header const *elf_header, Memory *elf_obj)
{
    Elf64ProgramHeader *prog_header = (Elf64ProgramHeader *)((uint8_t *)elf_header + elf_header->programs_offset);

    for (size_t i = 0; i < elf_header->programs_count; i++)
    {
        if (prog_header->type != ELF_PROGRAM_HEADER_LOAD)
        {
            log$("Ignoring segment with type={#0x}", prog_header->type);
            continue;
        }

        size_t size = align_up$(m_max(prog_header->memory_size, prog_header->file_size), MEM_PAGE_SIZE);

        if (!(prog_header->flags & ELF_PROGRAM_HEADER_WRITABLE) &&
            prog_header->file_size == prog_header->memory_size)
        {
            UNWRAP(space_map(task->space, elf_obj, prog_header->file_offset, size, prog_header->virtual_address, BR_MEM_USER));
        }
        else
        {
            HeapRange heap = UNWRAP(heap_alloc(size));
            uint8_t *ptr = (uint8_t *)heap.base;

            mem_set(ptr, 0, size);
            mem_cpy(ptr, (uint8_t *)elf_header + prog_header->file_offset, prog_header->file_size);

            Memory *prog_obj = memory_heap(heap);
            UNWRAP(space_map(task->space, prog_obj, 0, 0, prog_header->virtual_address, BR_MEM_USER | BR_MEM_WRITABLE));
            memory_deref(prog_obj);
        }

        prog_header = (Elf64ProgramHeader *)((uint8_t *)prog_header + elf_header->programs_size);
    }
}

void init_stack(Task *task)
{
    HeapRange heap = UNWRAP(heap_alloc(KERNEL_STACK_SIZE));
    Memory *obj = memory_heap(heap);
    space_map(task->space, obj, 0, 0, USER_STACK_BASE - KERNEL_STACK_SIZE, BR_MEM_USER | BR_MEM_WRITABLE);
    memory_deref(obj);
}

static uintptr_t init_pass(Task *task, Handover const *handover)
{
    HeapRange heap = UNWRAP(heap_alloc(align_up$(sizeof(Handover), MEM_PAGE_SIZE)));
    mem_cpy((void *)heap.base, handover, sizeof(Handover));
    Memory *obj = memory_heap(heap);
    uintptr_t addr = UNWRAP(space_map(task->space, obj, 0, 0, 0, BR_MEM_USER | BR_MEM_WRITABLE)).base;
    memory_deref(obj);

    return addr;
}

void init_start(Handover const *handover)
{
    Str name = str$("system");

    HandoverModule const *elf_module = handover_find_module(handover, name);

    if (elf_module == nullptr)
    {
        panic$("can't find '{}' handover module !", name);
    }

    Elf64Header *elf_header = (Elf64Header *)mmap_phys_to_io(elf_module->addr);
    Memory *elf_obj = memory_pmm((HeapRange){elf_module->addr, elf_module->size});

    assert_truth(elf_supported(elf_header, elf_module->size));

    Space *space = space_create(0);
    Task *task = UNWRAP(task_create(space, BR_RIGHT_IRQ | BR_RIGHT_DMA | BR_RIGHT_LOG | BR_RIGHT_TASK | BR_RIGHT_PIO, BR_TASK_USER));
    space_deref(space);

    elf_load_program(task, elf_header, elf_obj);

    init_stack(task);
    uintptr_t hoaddr = init_pass(task, handover);

    task_ref(task);

    sched_start(
        task,
        elf_header->entry,
        USER_STACK_BASE,
        (BrTaskArgs){
            .arg1 = hoaddr,
        });

    task_deref(task);
    memory_deref(elf_obj);
}
