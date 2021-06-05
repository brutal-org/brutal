#include <library/base.h>

#include "arch/arch.h"
#include "arch/asm.h"
#include "arch/stivale2.h"

static uint8_t stack[4096 * 4];

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0,
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0,
};

void stivale2_entry(struct stivale2_struct *info);

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    .entry_point = (uintptr_t)stivale2_entry,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1),
    .tags = (uintptr_t)&framebuffer_hdr_tag,
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;

    while (current_tag)
    {
        if (current_tag->identifier == id)
        {
            return current_tag;
        }

        current_tag = (void *)current_tag->next;
    }

    return nullptr;
}

void stivale2_entry(struct stivale2_struct *info)
{
    UNUSED(info);

    struct handover handover = {
        .raw = info,
    };

    arch_entry(&handover);

    for (;;)
    {
        cli();
        hlt();
    }
}
