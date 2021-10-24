#pragma once

#include <bal/abi/types.h>

BrResult brh_close(BrHandle handle);

BrGlobalInfo *brh_global_info(void);

BrLocalInfo *brh_local_info(void);
