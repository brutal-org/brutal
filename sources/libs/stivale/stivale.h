#pragma once

#include <bal/boot.h>
#include <stivale/spec.h>

void *stivale2_get_tag(struct stivale2_struct const *stivale2_struct, uint64_t id);

void stivale2_copy_to_handover(struct stivale2_struct const *info, Handover *handover);
