#pragma once

#include <bal/abi/types.h>

BrResult bal_unmap(BrHandle space, void *base, size_t len);

BrHandle bal_dup(BrHandle handle);

BrResult bal_close(BrHandle handle);

BrId bal_self_id(void);
