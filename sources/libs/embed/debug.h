#pragma once

#include <brutal/base.h>
#include <brutal/io.h>

void embed_log_prefix(IoWriter writer);

size_t embed_debug_backtrace(uintptr_t *buf, size_t cap);
