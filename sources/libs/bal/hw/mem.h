#pragma once

#include <bal/abi.h>
#include <brutal/base.h>
#include <brutal/text.h>
#include <ipc/pack.h>
#include <ipc/unpack.h>

typedef struct
{
    size_t off;
    size_t len;
    void *buf;
    BrHandle handle;
} BalMem;

MaybeError bal_mem_init(BalMem *self, size_t size);

MaybeError bal_mem_init_mobj(BalMem *self, BrHandle handle);

MaybeError bal_mem_init_mobj_offset(BalMem *self, BrHandle handle, size_t offset, size_t len);

MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size);

MaybeError bal_mem_deinit(BalMem *self);

void bal_mem_dup(BalMem *dst, BalMem *src);

BrResult bal_mem_map(BalMem *self);

BrResult bal_mem_unmap(BalMem *self);

MaybeError bal_mem_paddr(BalMem *self, uintptr_t *paddr);

void bal_mem_pack(IpcPack *pack, BalMem const *self);

void bal_mem_unpack(IpcUnpack *pack, BalMem *self);
