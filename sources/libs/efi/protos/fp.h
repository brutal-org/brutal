#pragma once

#include <efi/protos/dpp.h>

#define EFI_FILE_INFO_ID                                                               \
    {                                                                                  \
        0x09576e92, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }
#define EFI_FILE_SYSTEM_INFO_ID                                                        \
    {                                                                                  \
        0x09576e93, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }
#define EFI_FILE_SYSTEM_VOLUME_LABEL_ID                                                \
    {                                                                                  \
        0xdb47d7d3, 0xfe81, 0x11d3, { 0x9a, 0x35, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } \
    }

#define EFI_FILE_PROTOCOL_REVISION 0x00010000
#define EFI_FILE_PROTOCOL_REVISION2 0x00020000
#define EFI_FILE_PROTOCOL_LATEST_REVISION EFI_FILE_PROTOCOL_REVISION2

#define EFI_FILE_MODE_READ 0x0000000000000001
#define EFI_FILE_MODE_WRITE 0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

#define EFI_FILE_READ_ONLY 0x0000000000000001
#define EFI_FILE_HIDDEN 0x0000000000000002
#define EFI_FILE_SYSTEM 0x0000000000000004
#define EFI_FILE_RESERVED 0x0000000000000008
#define EFI_FILE_DIRECTORY 0x0000000000000010
#define EFI_FILE_ARCHIVE 0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037

typedef struct EFI_FILE_INFO
{
    uint64_t size;
    uint64_t file_size;
    uint64_t physical_size;
    EFITime create_time;
    EFITime last_access_time;
    EFITime modification_time;
    uint64_t attribute;
    char16 file_name[256];
} EFIFileInfo;

typedef struct EFI_FILE_SYSTEM_INFO
{
    uint64_t size;
    bool read_only;
    uint64_t volume_size;
    uint64_t free_space;
    uint32_t block_size;
    char16 volume_label;
} EFIFileSystemInfo;

typedef struct
{
    char16 volume_label[256];
} EFIFileSystemVolumeLabel;

typedef struct
{
    EFIEvent event;
    EFIStatus status;
    uint32_t buffer_size;
    void *buffer;
} EFIFileIOToken;

struct _EFI_FILE_IO;

#define DEF_FILE_EFI_FUNC(name, ...) typedef EFIStatus (*EFI_FILE_##name)(struct _EFI_FILE_IO * self __VA_OPT__(, ) __VA_ARGS__)

DEF_FILE_EFI_FUNC(OPEN, struct _EFI_FILE_IO **, char16 *, uint64_t, uint64_t);
DEF_FILE_EFI_FUNC(CLOSE);
DEF_FILE_EFI_FUNC(DELETE);
DEF_FILE_EFI_FUNC(READ, uint64_t *, void *);
DEF_FILE_EFI_FUNC(WRITE, uint8_t, void *);
DEF_FILE_EFI_FUNC(GET_POSITION, uint64_t *);
DEF_FILE_EFI_FUNC(SET_POSITION, uint64_t);
DEF_FILE_EFI_FUNC(GET_INFO, EFIGUID *, uint64_t *, void *);
DEF_FILE_EFI_FUNC(SET_INFO, EFIGUID *, uint64_t, void *);
DEF_FILE_EFI_FUNC(FLUSH);
DEF_FILE_EFI_FUNC(OPEN_EX, struct _EFI_FILE_IO **, char16 *, uint64_t, uint64_t, EFIFileIOToken *);
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
} EFIFileProtocol;
