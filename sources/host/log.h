#pragma once

#include <brutal/io.h>

void host_log_lock(void);

void host_log_unlock(void);

struct writer *host_log_writer(void);
