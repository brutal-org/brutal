#pragma once

#include <bal/abi/types.h>

BrResult bal_close(BrHandle handle);

BrResult bal_unmap(BrHandle space, void *base, size_t len);

BrId bal_self_id(void);
