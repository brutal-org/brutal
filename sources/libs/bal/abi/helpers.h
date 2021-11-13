#pragma once

#include <bal/abi/types.h>

BrResult bal_close(BrHandle handle);

BrResult bal_unmap(BrSpace space, void *base, size_t len);
