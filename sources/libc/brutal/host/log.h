#pragma once

#include <brutal/io.h>

void host_log_lock(void);

void host_log_unlock(void);

noreturn void host_log_panic(void);

IoWriter *host_log_writer(void);
