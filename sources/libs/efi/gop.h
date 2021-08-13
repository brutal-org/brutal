#pragma once
#include <efi/base.h>

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID                                              \
    {                                                                                  \
        0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } \
    }

typedef struct
{
    u32 red_mask;
    u32 green_mask;
    u32 blue_mask;
    u32 reserved_mask;
} EFIPixelBitmap;

typedef enum
{
    PIXEL_RGB_RESERVED_8_BIT,
    PIXEL_BGR_8_BIT,
    PIXEL_BIT_MASK,
    PIXEL_BLT_ONLY,
    PIXEL_FORMAT_MAX
} EFIGraphicsPixelFormat;

typedef struct
{
    u32 version;
    u32 horizontal_resolution;
    u32 vertical_resolution;
    EFIGraphicsPixelFormat pixel_format;
    EFIPixelBitmap pixel_information;
    u32 pixels_per_scan_line;
} EFIGraphicsOutputModeInfo;

typedef struct
{
    u32 max_mode;
    u32 mode;
    EFIGraphicsOutputModeInfo *info;
    u64 size_of_info;
    u64 framebuffer_base;
    u64 framebuffer_size;
} EFIGraphicsOutputProtocolMode;

typedef struct
{
    u8 blue;
    u8 green;
    u8 red;
    u8 reserved;
} EFIGraphicsOutputBltPixel;

typedef enum
{
    BLT_VIDEO_FILL,
    BLT_VIDEO_TO_BLT_BUFFER,
    BLT_BUFFER_TO_VIDEO,
    BLT_VIDEO_TO_VIDEO,
    BLT_OPERATION_MAX
} EFIGraphicsOutputBltOperation;

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL;

#define DEF_GOP_EFI_FUNC(name, ...) typedef EFIStatus (*EFI_GRAPHICS_OUTPUT_PROTOCOL_##name)(struct _EFI_GRAPHICS_OUTPUT_PROTOCOL * self __VA_OPT__(, ) __VA_ARGS__)

DEF_GOP_EFI_FUNC(QUERY_MODE, u32, u64 *, EFIGraphicsOutputModeInfo **);
DEF_GOP_EFI_FUNC(SET_MODE, u32);
DEF_GOP_EFI_FUNC(BLT, EFIGraphicsOutputBltPixel *, EFIGraphicsOutputBltOperation, u64 s_x, u64 s_y, u64 d_x, u64 d_y, u64 width, u64 height, u64 delta);

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE query_mode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE set_mode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT blt;
    EFIGraphicsOutputProtocolMode *mode;
} EFIGraphicsOutputProtocol;
