#pragma once

#include <bal/abi/types.h>

BrResult brh_close(BrHandle handle);

BrResult brh_unmap(BrSpace space, void* base, size_t len);

BrGlobalInfo *brh_global_info(void);

BrLocalInfo *brh_local_info(void);
