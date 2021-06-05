#pragma once

#include <library/io/write.h>

void host_log_lock(void);

struct writer *host_log_writer(void);

void host_log_unlock(void);
