#include <bal/abi.h>
#include <bal/task/task.h>
#include <elf/elf.h>
#include <brutal-debug>

static void bal_exec_load(BrHandle space, Elf64Header const *elf_header, BrHandle elf_memory)
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
            BrMapArgs prog_map = {
                .space = space,
                .memory = elf_memory,
                .offset = prog_header->file_offset,
                .size = size,
                .vaddr = prog_header->virtual_address,
            };

            assert_br_success(br_map(&prog_map));
        }
        else
        {
            BrCreateArgs prog_obj = {
                .type = BR_OBJECT_MEMORY,
                .memory = {
                    .size = size,
                },
            };

            assert_br_success(br_create(&prog_obj));

            BrMapArgs prog_local_map = {
                .space = BR_HANDLE_SELF,
                .memory = prog_obj.handle,
                .flags = BR_MEM_WRITABLE,
            };

            assert_br_success(br_map(&prog_local_map));

            uint8_t *ptr = (uint8_t *)prog_local_map.vaddr;

            mem_set(ptr, 0, size);
            mem_cpy(ptr, (uint8_t *)elf_header + prog_header->file_offset, prog_header->file_size);

            BrMapArgs prog_map = {
                .space = space,
                .memory = prog_obj.handle,
                .offset = 0,
                .size = 0,
                .vaddr = prog_header->virtual_address,
                .flags = BR_MEM_WRITABLE,
            };

            assert_br_success(br_map(&prog_map));

            assert_br_success(br_unmap(&(BrUnmapArgs){
                .space = BR_HANDLE_SELF,
                .vaddr = prog_local_map.vaddr,
                .size = size,
            }));

            assert_br_success(bal_close(prog_obj.handle));
        }

        prog_header = (Elf64ProgramHeader *)((uint8_t *)prog_header + elf_header->programs_size);
    }
}

static uintptr_t bal_exec_stack(BrHandle space, IpcCap *caps, size_t count, BrTaskArgs *start_args)
{
    BrCreateArgs stack_obj = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = 0x4000,
        },
    };

    assert_br_success(br_create(&stack_obj));

    BrMapArgs local_stack_map = {
        .space = BR_HANDLE_SELF,
        .memory = stack_obj.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&local_stack_map));

    uint8_t *base = (uint8_t *)local_stack_map.vaddr + 0x4000;
    uint8_t *head = base;

    head -= align_up$(sizeof(IpcCap) * count, 16);
    mem_cpy(head, caps, align_up$(sizeof(IpcCap) * count, 16));

    start_args->arg1 = 0xC0000000 - ((uintptr_t)base - (uintptr_t)head);
    start_args->arg2 = count;

    uintptr_t sp = 0xC0000000 - ((uintptr_t)base - (uintptr_t)head);

    BrMapArgs stack_map = {
        .space = space,
        .memory = stack_obj.handle,
        .vaddr = 0xC0000000 - 0x4000,
        .flags = BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&stack_map));

    BrUnmapArgs local_stack_unmap = {
        .space = BR_HANDLE_SELF,
        .vaddr = local_stack_map.vaddr,
        .size = local_stack_map.size,
    };

    assert_br_success(br_unmap(&local_stack_unmap));

    assert_br_success(bal_close(stack_obj.handle));

    return sp;
}

BrResult bal_TaskExec(BalTask *task, BalMem *elf, BrRight rights, IpcCap *caps, size_t count)
{
    BrCreateArgs space = {
        .type = BR_OBJECT_SPACE,
    };

    assert_br_success(br_create(&space));

    BrCreateArgs elf_task = {
        .type = BR_OBJECT_TASK,
        .task = {
            .space = space.handle,
            .rights = rights,
        },
    };

    assert_br_success(br_create(&elf_task));

    Elf64Header *header = (Elf64Header *)elf->buf;

    bal_exec_load(space.handle, header, elf->handle);

    BrTaskArgs task_args;
    uintptr_t sp = bal_exec_stack(space.handle, caps, count, &task_args);

    BrStartArgs elf_start = {
        .handle = elf_task.handle,
        .ip = header->entry,
        .args = task_args,
        .sp = sp,
    };

    assert_br_success(br_start(&elf_start));

    assert_br_success(bal_close(space.handle));

    task->id = elf_task.id;
    task->handle = elf_task.handle;

    return BR_SUCCESS;
}
