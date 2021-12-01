#pragma once

#include <loader/protocol.h>
#include <stdint.h>
#include "kernel/kernel.h"

Handover arch_create_handover(uint64_t fdt_addr);
