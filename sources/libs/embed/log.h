#pragma once

#include <brutal-io>

void embed_log_lock(void);

void embed_log_unlock(void);

noreturn void embed_log_panic(void);

IoWriter embed_log_writer(void);
