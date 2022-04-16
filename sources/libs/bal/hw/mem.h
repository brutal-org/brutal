#pragma once

#include <bal/abi.h>
#include <ipc/pack.h>
#include <ipc/unpack.h>
#include <brutal-base>
#include <brutal-text>

typedef struct
{
    size_t off;
    size_t len;
    void *buf;
    BrHandle handle;
} BalMem;

MaybeError bal_mem_init(BalMem *self, size_t size);

MaybeError bal_mem_init_mobj(BalMem *self, BrHandle handle);

/**
 * @brief Create a new memory object from a memory handle with an offset and a length.
 * @note The ownerships of the handle is given to the memory object
 * @param self The memory object to create.
 * @param handle The handle to give to the created memory object.
 * @param offset, len The offset and the length of the memory object to create.
 * @returns This function may return an error if we weren't able to map the \p handle.
 */
MaybeError bal_mem_init_mobj_offset(BalMem *self, BrHandle handle, size_t offset, size_t len);

/**
 * @brief Create a new memory object with a specific physical address and size.
 * @note This also map the physical memory address. You can access it using `BalMem.buf`
 * @param self The memory object to create.
 * @param addr, size The physical memory range to use when creating the object.
 * @returns This function may return an error if we weren't able to map or create the memory.
 */
MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size);

MaybeError bal_mem_deinit(BalMem *self);

void bal_mem_dup(BalMem *dst, BalMem *src);

BrResult bal_mem_map(BalMem *self);

BrResult bal_mem_unmap(BalMem *self);

MaybeError bal_mem_paddr(BalMem *self, uintptr_t *paddr);

void bal_mem_pack(IpcPack *pack, BalMem const *self);

void bal_mem_unpack(IpcUnpack *pack, BalMem *self);
