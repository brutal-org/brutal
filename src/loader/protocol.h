#pragma once

#include <bal/boot.h>
#include <efi/protos.h>
#include "loader/config.h"
#include "loader/memory.h"

typedef void EntryPointFn(void *handover, uint64_t magic) __attribute__((sysv_abi));

EntryPointFn *efi_populate_handover(LoaderEntry const *entry, Handover *ho, VmmSpace vmm);
