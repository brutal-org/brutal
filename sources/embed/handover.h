#pragma once
#include <bal/boot.h>

void embed_handover_fill_mmap(HandoverMmap *self);
void embed_handover_fill_framebuffer(HandoverFramebuffer *fb, uint64_t requested_width, uint64_t requested_height);
uintptr_t embed_handover_get_rsdp();
