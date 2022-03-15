#ifndef __STIVALE__STIVALE2_H__
#define __STIVALE__STIVALE2_H__

#include <stdint.h>

#if (defined(_STIVALE2_SPLIT_64) && defined(__i386__)) || defined(_STIVALE2_SPLIT_64_FORCE)

#    define _stivale2_split64(NAME) \
        union                       \
        {                           \
            uint32_t NAME;          \
            uint32_t NAME##_lo;     \
        };                          \
        uint32_t NAME##_hi

#else

#    define _stivale2_split64(NAME) \
        uint64_t NAME

#endif

// Anchor for non ELF kernels
struct stivale2_anchor
{
    uint8_t anchor[15];
    uint8_t bits;
    _stivale2_split64(phys_load_addr);
    _stivale2_split64(phys_bss_start);
    _stivale2_split64(phys_bss_end);
    _stivale2_split64(phys_stivale2hdr);
};

struct stivale2_tag
{
    uint64_t identifier;
    _stivale2_split64(next);
};

/* --- Header --------------------------------------------------------------- */
/*  Information passed from the kernel to the bootloader                      */

struct stivale2_header
{
    _stivale2_split64(entry_point);
    _stivale2_split64(stack);
    uint64_t flags;
    _stivale2_split64(tags);
};

#define STIVALE2_HEADER_TAG_ANY_VIDEO_ID 0xc75c9fa92a44c4db

struct stivale2_header_tag_any_video
{
    struct stivale2_tag tag;
    uint64_t preference;
};

#define STIVALE2_HEADER_TAG_FRAMEBUFFER_ID 0x3ecc1bc43d0f7971

struct stivale2_header_tag_framebuffer
{
    struct stivale2_tag tag;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint16_t unused;
};

#define STIVALE2_HEADER_TAG_FB_MTRR_ID 0x4c7bb07731282e00

#define STIVALE2_HEADER_TAG_SLIDE_HHDM_ID 0xdc29269c2af53d1d

struct stivale2_header_tag_slide_hhdm
{
    struct stivale2_tag tag;
    uint64_t flags;
    _stivale2_split64(alignment);
};

#define STIVALE2_HEADER_TAG_TERMINAL_ID 0xa85d499b1823be72

struct stivale2_header_tag_terminal
{
    struct stivale2_tag tag;
    uint64_t flags;
    _stivale2_split64(callback);
};

#define STIVALE2_TERM_CB_DEC 10
#define STIVALE2_TERM_CB_BELL 20
#define STIVALE2_TERM_CB_PRIVATE_ID 30
#define STIVALE2_TERM_CB_STATUS_REPORT 40
#define STIVALE2_TERM_CB_POS_REPORT 50
#define STIVALE2_TERM_CB_KBD_LEDS 60
#define STIVALE2_TERM_CB_MODE 70
#define STIVALE2_TERM_CB_LINUX 80

#define STIVALE2_TERM_CTX_SIZE ((uint64_t)(-1))
#define STIVALE2_TERM_CTX_SAVE ((uint64_t)(-2))
#define STIVALE2_TERM_CTX_RESTORE ((uint64_t)(-3))
#define STIVALE2_TERM_FULL_REFRESH ((uint64_t)(-4))

#define STIVALE2_HEADER_TAG_SMP_ID 0x1ab015085f3273df

struct stivale2_header_tag_smp
{
    struct stivale2_tag tag;
    uint64_t flags;
};

#define STIVALE2_HEADER_TAG_5LV_PAGING_ID 0x932f477032007e8f

#define STIVALE2_HEADER_TAG_UNMAP_NULL_ID 0x92919432b16fe7e7

/* --- Struct --------------------------------------------------------------- */
/*  Information passed from the bootloader to the kernel                      */

struct stivale2_struct
{
#define STIVALE2_BOOTLOADER_BRAND_SIZE 64
    char bootloader_brand[STIVALE2_BOOTLOADER_BRAND_SIZE];

#define STIVALE2_BOOTLOADER_VERSION_SIZE 64
    char bootloader_version[STIVALE2_BOOTLOADER_VERSION_SIZE];

    uint64_t tags;
};

#define STIVALE2_STRUCT_TAG_PMRS_ID 0x5df266a64047b6bd

#define STIVALE2_PMR_EXECUTABLE ((uint64_t)1 << 0)
#define STIVALE2_PMR_WRITABLE ((uint64_t)1 << 1)
#define STIVALE2_PMR_READABLE ((uint64_t)1 << 2)

struct stivale2_pmr
{
    uint64_t base;
    uint64_t length;
    uint64_t permissions;
};

struct stivale2_struct_tag_pmrs
{
    struct stivale2_tag tag;
    uint64_t entries;
    struct stivale2_pmr pmrs[];
};

#define STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID 0x060d78874a2a8af0

struct stivale2_struct_tag_kernel_base_address
{
    struct stivale2_tag tag;
    uint64_t physical_base_address;
    uint64_t virtual_base_address;
};

#define STIVALE2_STRUCT_TAG_CMDLINE_ID 0xe5e76a1b4597a781

struct stivale2_struct_tag_cmdline
{
    struct stivale2_tag tag;
    uint64_t cmdline;
};

#define STIVALE2_STRUCT_TAG_MEMMAP_ID 0x2187f79e8612de07

#define STIVALE2_MMAP_USABLE 1
#define STIVALE2_MMAP_RESERVED 2
#define STIVALE2_MMAP_ACPI_RECLAIMABLE 3
#define STIVALE2_MMAP_ACPI_NVS 4
#define STIVALE2_MMAP_BAD_MEMORY 5
#define STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE 0x1000
#define STIVALE2_MMAP_KERNEL_AND_MODULES 0x1001
#define STIVALE2_MMAP_FRAMEBUFFER 0x1002

struct stivale2_mmap_entry
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
};

struct stivale2_struct_tag_memmap
{
    struct stivale2_tag tag;
    uint64_t entries;
    struct stivale2_mmap_entry memmap[];
};

#define STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID 0x506461d2950408fa

#define STIVALE2_FBUF_MMODEL_RGB 1

struct stivale2_struct_tag_framebuffer
{
    struct stivale2_tag tag;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused;
};

#define STIVALE2_STRUCT_TAG_EDID_ID 0x968609d7af96b845

struct stivale2_struct_tag_edid
{
    struct stivale2_tag tag;
    uint64_t edid_size;
    uint8_t edid_information[];
};

#define STIVALE2_STRUCT_TAG_TEXTMODE_ID 0x38d74c23e0dca893

struct stivale2_struct_tag_textmode
{
    struct stivale2_tag tag;
    uint64_t address;
    uint16_t unused;
    uint16_t rows;
    uint16_t cols;
    uint16_t bytes_per_char;
};

#define STIVALE2_STRUCT_TAG_FB_MTRR_ID 0x6bc1a78ebe871172

#define STIVALE2_STRUCT_TAG_TERMINAL_ID 0xc2b3f4c3233b0974

struct stivale2_struct_tag_terminal
{
    struct stivale2_tag tag;
    uint32_t flags;
    uint16_t cols;
    uint16_t rows;
    uint64_t term_write;
    uint64_t max_length;
};

#define STIVALE2_STRUCT_TAG_MODULES_ID 0x4b6fe466aade04ce

struct stivale2_module
{
    uint64_t begin;
    uint64_t end;

#define STIVALE2_MODULE_STRING_SIZE 128
    char string[STIVALE2_MODULE_STRING_SIZE];
};

struct stivale2_struct_tag_modules
{
    struct stivale2_tag tag;
    uint64_t module_count;
    struct stivale2_module modules[];
};

#define STIVALE2_STRUCT_TAG_RSDP_ID 0x9e1786930a375e78

struct stivale2_struct_tag_rsdp
{
    struct stivale2_tag tag;
    uint64_t rsdp;
};

#define STIVALE2_STRUCT_TAG_EPOCH_ID 0x566a7bed888e1407

struct stivale2_struct_tag_epoch
{
    struct stivale2_tag tag;
    uint64_t epoch;
};

#define STIVALE2_STRUCT_TAG_FIRMWARE_ID 0x359d837855e3858c

#define STIVALE2_FIRMWARE_BIOS (1 << 0)

struct stivale2_struct_tag_firmware
{
    struct stivale2_tag tag;
    uint64_t flags;
};

#define STIVALE2_STRUCT_TAG_EFI_SYSTEM_TABLE_ID 0x4bc5ec15845b558e

struct stivale2_struct_tag_efi_system_table
{
    struct stivale2_tag tag;
    uint64_t system_table;
};

#define STIVALE2_STRUCT_TAG_KERNEL_FILE_ID 0xe599d90c2975584a

struct stivale2_struct_tag_kernel_file
{
    struct stivale2_tag tag;
    uint64_t kernel_file;
};

#define STIVALE2_STRUCT_TAG_KERNEL_FILE_V2_ID 0x37c13018a02c6ea2

struct stivale2_struct_tag_kernel_file_v2
{
    struct stivale2_tag tag;
    uint64_t kernel_file;
    uint64_t kernel_size;
};

#define STIVALE2_STRUCT_TAG_BOOT_VOLUME_ID 0x9b4358364c19ee62

struct stivale2_guid
{
    uint32_t a;
    uint16_t b;
    uint16_t c;
    uint8_t d[8];
};

struct stivale2_struct_tag_boot_volume
{
    struct stivale2_tag tag;
    uint64_t flags;
    struct stivale2_guid guid;
    struct stivale2_guid part_guid;
};

#define STIVALE2_STRUCT_TAG_KERNEL_SLIDE_ID 0xee80847d01506c57

struct stivale2_struct_tag_kernel_slide
{
    struct stivale2_tag tag;
    uint64_t kernel_slide;
};

#define STIVALE2_STRUCT_TAG_SMBIOS_ID 0x274bd246c62bf7d1

struct stivale2_struct_tag_smbios
{
    struct stivale2_tag tag;
    uint64_t flags;
    uint64_t smbios_entry_32;
    uint64_t smbios_entry_64;
};

#define STIVALE2_STRUCT_TAG_SMP_ID 0x34d1d96339647025

struct stivale2_smp_info
{
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t target_stack;
    uint64_t goto_address;
    uint64_t extra_argument;
};

struct stivale2_struct_tag_smp
{
    struct stivale2_tag tag;
    uint64_t flags;
    uint32_t bsp_lapic_id;
    uint32_t unused;
    uint64_t cpu_count;
    struct stivale2_smp_info smp_info[];
};

#define STIVALE2_STRUCT_TAG_PXE_SERVER_INFO 0x29d1e96239247032

struct stivale2_struct_tag_pxe_server_info
{
    struct stivale2_tag tag;
    uint32_t server_ip;
};

#define STIVALE2_STRUCT_TAG_MMIO32_UART 0xb813f9b8dbc78797

struct stivale2_struct_tag_mmio32_uart
{
    struct stivale2_tag tag;
    uint64_t addr;
};

#define STIVALE2_STRUCT_TAG_DTB 0xabb29bd49a2833fa

struct stivale2_struct_tag_dtb
{
    struct stivale2_tag tag;
    uint64_t addr;
    uint64_t size;
};

#define STIVALE2_STRUCT_TAG_HHDM_ID 0xb0ed257db18cb58f

struct stivale2_struct_tag_hhdm
{
    struct stivale2_tag tag;
    uint64_t addr;
};

#undef _stivale2_split64

#endif
