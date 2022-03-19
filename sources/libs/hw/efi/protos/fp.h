#pragma once

#include <efi/protos/dpp.h>

#define EFI_FILE_INFO_ID                                                               \
    (EfiGuid)                                                                          \
    {                                                                                  \
        0x09576e92, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

#define EFI_FILE_SYSTEM_INFO_ID                                                        \
    (EfiGuid)                                                                          \
    {                                                                                  \
        0x09576e93, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

#define EFI_FILE_SYSTEM_VOLUME_LABEL_ID                                                \
    (EfiGuid)                                                                          \
    {                                                                                  \
        0xdb47d7d3, 0xfe81, 0x11d3, { 0x9a, 0x35, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } \
    }

#define EFI_FILE_MODE_READ (1 << 0)
#define EFI_FILE_MODE_WRITE (1 << 1)
#define EFI_FILE_MODE_CREATE (1ull << 63)

#define EFI_FILE_READ_ONLY (1 << 0)
#define EFI_FILE_HIDDEN (1 << 1)
#define EFI_FILE_SYSTEM (1 << 2)
#define EFI_FILE_RESERVED (1 << 3)
#define EFI_FILE_DIRECTORY (1 << 4)
#define EFI_FILE_ARCHIVE (1 << 5)
#define EFI_FILE_VALID_ATTR 0x37

typedef struct EFI_FILE_INFO
{
    uint64_t size;
    uint64_t file_size;
    uint64_t physical_size;
    EFITime create_time;
    EFITime last_access_time;
    EFITime modification_time;
    uint64_t attribute;
    uint16_t file_name[256];
} EFIFileInfo;

typedef struct EFI_FILE_SYSTEM_INFO
{
    uint64_t size;
    bool read_only;
    uint64_t volume_size;
    uint64_t free_space;
    uint32_t block_size;
    uint16_t volume_label;
} EFIFileSystemInfo;

typedef struct
{
    uint16_t volume_label[256];
} EFIFileSystemVolumeLabel;

typedef struct
{
    EFIEvent event;
    EfiStatus status;
    uint32_t buffer_size;
    void *buffer;
} EFIFileIOToken;

struct _EFI_FILE_IO;

#define DEF_FILE_EFI_FUNC(name, ...) typedef EfiStatus (*EFI_FILE_##name)(struct _EFI_FILE_IO * self IFN(__VA_ARGS__)(, ) __VA_ARGS__)

DEF_FILE_EFI_FUNC(OPEN, struct _EFI_FILE_IO **, uint16_t *, uint64_t, uint64_t);
DEF_FILE_EFI_FUNC(CLOSE);
DEF_FILE_EFI_FUNC(DELETE);
DEF_FILE_EFI_FUNC(READ, uint64_t *, void *);
DEF_FILE_EFI_FUNC(WRITE, uint8_t, void *);
DEF_FILE_EFI_FUNC(GET_POSITION, uint64_t *);
DEF_FILE_EFI_FUNC(SET_POSITION, uint64_t);
DEF_FILE_EFI_FUNC(GET_INFO, EfiGuid *, uint64_t *, void *);
DEF_FILE_EFI_FUNC(SET_INFO, EfiGuid *, uint64_t, void *);
DEF_FILE_EFI_FUNC(FLUSH);
DEF_FILE_EFI_FUNC(OPEN_EX, struct _EFI_FILE_IO **, uint16_t *, uint64_t, uint64_t, EFIFileIOToken *);
DEF_FILE_EFI_FUNC(READ_EX, EFIFileIOToken *);
DEF_FILE_EFI_FUNC(WRITE_EX, EFIFileIOToken *);
DEF_FILE_EFI_FUNC(FLUSH_EX, EFIFileIOToken *);

typedef struct _EFI_FILE_IO
{
    uint64_t revision;
    EFI_FILE_OPEN open;
    EFI_FILE_CLOSE close;
    EFI_FILE_DELETE delete_file;
    EFI_FILE_READ read;
    EFI_FILE_WRITE write;
    EFI_FILE_GET_POSITION get_position;
    EFI_FILE_SET_POSITION set_position;
    EFI_FILE_GET_INFO get_info;
    EFI_FILE_SET_INFO set_info;
    EFI_FILE_FLUSH flush;
    EFI_FILE_OPEN_EX open_ex;
    EFI_FILE_READ_EX read_ex;
    EFI_FILE_WRITE_EX write_ex;
    EFI_FILE_FLUSH_EX flush_ex;
} EfiFileProtocol;
