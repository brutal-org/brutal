#pragma once

#include <efi/protos/fp.h>
#include <efi/types.h>

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION 0x00010000

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID                                            \
    {                                                                                   \
        0x0964e5b22, 0x6459, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EfiStatus (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *, EfiFileProtocol **Root);

typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
{
    uint64_t revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME open_volume;
} EFISimpleFileSystemProtocol;
