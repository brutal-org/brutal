#pragma once

#include <efi/types.h>

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID                                              \
    {                                                                                  \
        0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } \
    }

typedef struct
{
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t reserved_mask;
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
    uint32_t version;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    EFIGraphicsPixelFormat pixel_format;
    EFIPixelBitmap pixel_information;
    uint32_t pixels_per_scan_line;
} EFIGraphicsOutputModeInfo;

typedef struct
{
    uint32_t max_mode;
    uint32_t mode;
    EFIGraphicsOutputModeInfo *info;
    uint64_t size_of_info;
    uint64_t framebuffer_base;
    uint64_t framebuffer_size;
} EFIGraphicsOutputProtocolMode;

typedef struct
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t _reserved;
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

#define DEF_GOP_EFI_FUNC(name, ...) typedef EfiStatus (*EFI_GRAPHICS_OUTPUT_PROTOCOL_##name)(struct _EFI_GRAPHICS_OUTPUT_PROTOCOL * self IFN(__VA_ARGS__)(, ) __VA_ARGS__)

DEF_GOP_EFI_FUNC(QUERY_MODE, uint32_t, uint64_t *, EFIGraphicsOutputModeInfo **);
DEF_GOP_EFI_FUNC(SET_MODE, uint32_t);
DEF_GOP_EFI_FUNC(BLT, EFIGraphicsOutputBltPixel *, EFIGraphicsOutputBltOperation, uint64_t s_x, uint64_t s_y, uint64_t d_x, uint64_t d_y, uint64_t width, uint64_t height, uint64_t delta);

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE query_mode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE set_mode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT blt;
    EFIGraphicsOutputProtocolMode *mode;
} EFIGraphicsOutputProtocol;
