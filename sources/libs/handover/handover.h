
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Definitions ---------------------------------------------------------- */

#define HANDOVER_MAGIC (0xc001b001)

#define HANDOVER_FREE (0)

#define HANDOVER_SELF (0xa24f988d)

#define HANDOVER_STACK (0xf65b391b)

#define HANDOVER_KERNEL (0xbfc71b20)

#define HANDOVER_FILE (0xcbc36d3b)

#define HANDOVER_RSDP (0x8d3bbb)

#define HANDOVER_FDT (0xb628bbc1)

#define HANDOVER_FB (0xe2d55685)
#define HANDOVER_FB_RGBX32 (0x7451)
#define HANDOVER_FB_BGRX32 (0xd040)

#define HANDOVER_END (0xffffffff)

typedef struct
{
    uint32_t type;
    uint32_t flags;
    uint64_t start;
    uint64_t size;

    union
    {
        struct
        {
            uint16_t width;
            uint16_t height;
            uint16_t pitch;
            uint16_t format;
        } fb;

        struct
        {
            uint32_t name;
            uint32_t meta;
        } file;

        uint64_t more;
    };
} HandoverRecord;

typedef struct
{
    uint32_t magic, agent, size, count;
    HandoverRecord records[];
} Handover;

typedef void HandoverEntry(uint64_t magic, Handover const *handover);

#ifdef HANDOVER_INCLUDE_HEADER

/* --- Header --------------------------------------------------------------- */

#    define HANDOVER_SECTION ".handover.header"

typedef struct
{
    uint32_t type;
    uint32_t flags;
    uint64_t more;
} HandoverRequest;

#    ifdef HANDOVER_INCLUDE_MACROS

#        define HANDOVER_REQ_START     \
            {                          \
                .type = HANDOVER_MAGIC \
            }

#        define HANDOVER_REQ_END     \
            {                        \
                .type = HANDOVER_END \
            }

#        define WITH_FB             \
            {                       \
                .type = HANDOVER_FB \
            }

#        define WITH_ACPI             \
            {                         \
                .type = HANDOVER_RSDP \
            }

#        define WITH_FDT             \
            {                        \
                .type = HANDOVER_FDT \
            }

#        define WITH_FILES            \
            {                         \
                .type = HANDOVER_FILE \
            }

#        define HANDOVER(...)                                                        \
            __attribute__((section(HANDOVER_SECTION),                                \
                           used)) static HandoverRequest const handover_header[] = { \
                {.type = HANDOVER_MAGIC},                                            \
                __VA_ARGS__ __VA_OPT__(, ){.type = HANDOVER_END},                    \
            };

#    endif

#endif

#ifdef HANDOVER_INCLUDE_UTILITIES

/* --- Utilities ------------------------------------------------------------ */

static inline size_t handover_strlen(const char *str)
{
    size_t len = 0;
    while (*str++)
        len++;
    return len;
}

static inline bool handover_streq(const char *str1, const char *str2)
{
    while (*str1 && *str2)
    {
        if (*str1++ != *str2++)
            return false;
    }
    return *str1 == *str2;
}

static inline bool handover_is_ordered(Handover const *handover)
{
    for (size_t i = 0; i < handover->count - 1; i++)
    {
        if (handover->records[i].start > handover->records[i + 1].start)
        {
            return false;
        }
    }
    return true;
}

static inline bool handover_valid(Handover const *handover)
{
    return (handover->magic == HANDOVER_MAGIC) &&
           (handover->records[handover->count].type == HANDOVER_END) &&
           handover_is_ordered(handover);
}

static inline char const *handover_string(Handover const *self,
                                          uint64_t offset)
{
    return (char const *)(self + 1) + sizeof(HandoverRecord) * self->count +
           offset;
}

static inline HandoverRecord *handover_record(Handover const *self,
                                              uint32_t index)
{
    return (HandoverRecord *)(self + 1) + index;
}

static inline HandoverRecord *handover_record_by_type(Handover const *self,
                                                      uint32_t type)
{
    for (uint32_t i = 0; i < self->count; ++i)
    {
        HandoverRecord *record = handover_record(self, i);
        if (record->type == type)
            return record;
    }

    return NULL;
}

static inline HandoverRecord *handover_file(Handover *self, char const *name)
{
    for (uint32_t i = 0; i < self->count; ++i)
    {
        HandoverRecord *record = handover_record(self, i);
        char const *ho_name = handover_string(self, record->file.name);

        if (record->type == HANDOVER_FILE && !handover_streq(name, ho_name))
        {
            return record;
        }
    }

    return NULL;
}

static inline char const *handover_type_to_string(uint32_t type)
{
    switch (type)
    {
    case HANDOVER_FREE:
        return "free";

    case HANDOVER_SELF:
        return "self";

    case HANDOVER_STACK:
        return "stack";

    case HANDOVER_KERNEL:
        return "kernel";

    case HANDOVER_FILE:
        return "file";

    case HANDOVER_RSDP:
        return "rsdp";

    case HANDOVER_FDT:
        return "fdt";

    case HANDOVER_FB:
        return "fb";

    default:
        return "unknown";
    }
}

static inline bool handover_dump(Handover *self, void (*print)(void *ctx, const char *fmt, ...), void *ctx)
{
    if (!handover_valid(self))
    {
        print(ctx, "valid: false\n");
        return false;
    }

    print(ctx, "valid: true\n");
    print(ctx, "magic: 0x%x\n", self->magic);

    char const *ho_agent = handover_string(self, self->agent);

    if (!ho_agent)
    {
        print(ctx, "agent: anonymous\n");
        return false;
    }
    else
    {
        print(ctx, "agent: '%s'\n", ho_agent);
    }

    print(ctx, "len: %u\n", self->len);
    print(ctx, "count: %u\n", self->count);

    for (uint32_t i = 0; i < self->count; ++i)
    {
        HandoverRecord *record = handover_record(self, i);

        print(ctx, "record[%u]:\n", i);
        print(ctx, "  type: %s\n", handover_type_to_string(record->type));
        print(ctx, "  flags: 0x%x\n", record->flags);
        print(ctx, "  start: 0x%x\n", record->start);
        print(ctx, "  size: 0x%x\n", record->size);

        switch (record->type)
        {
        case HANDOVER_FB:
            print(ctx, "  fb:\n");
            print(ctx, "    width: %u\n", record->fb.width);
            print(ctx, "    height: %u\n", record->fb.height);
            print(ctx, "    pitch: %u\n", record->fb.pitch);
            print(ctx, "    format: %u\n", record->fb.format);
            break;

        case HANDOVER_FILE:
            print(ctx, "  file:\n");
            print(ctx, "    name: '%s'\n", handover_string(self, record->file.name));
            print(ctx, "    meta: '%s'\n", handover_string(self, record->file.meta));
            break;

        default:
            print(ctx, "  more: 0x%llx\n", record->more);
            break;
        }
    }

    print(ctx, "strings:\n");

    char const *str = handover_string(self, 0);
    while (str < (char const *)(self) + self->len)
    {
        print(ctx, "  '%s'\n", str);
        str += handover_strlen(str) + 1;
    }

    return true;
}

#endif
