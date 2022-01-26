#include <brutal/codec/gzip/gzip.h>
#include <brutal/gfx/ssfn2/ssfn.h>
#include <string.h>

/* magic bytes */
#define SSFN_MAGIC "SFN2"
#define SSFN_COLLECTION "SFNC"
#define SSFN_ENDMAGIC "2NFS"

typedef struct PACKED
{
    le_uint8_t magic[4];         /* SSFN magic bytes */
    le_uint32_t size;            /* total size in bytes */
} SSFN2CommonHeader;

typedef struct PACKED
{
    le_uint8_t type;             /* font family and style */
    le_uint8_t features;         /* format features and revision */
    le_uint8_t width;            /* overall width of the font */
    le_uint8_t height;           /* overall height of the font */
    le_uint8_t baseline;         /* horizontal baseline in grid pixels */
    le_uint8_t underline;        /* position of under line in grid pixels */
    le_uint16_t fragments_offs;  /* offset of fragments table */
    le_uint32_t characters_offs; /* characters table offset */
    le_uint32_t ligature_offs;   /* ligatures table offset */
    le_uint32_t kerning_offs;    /* kerning table offset */
    le_uint32_t cmap_offs;       /* color map offset */
} SSFN2FontHeader;

typedef enum
{
    FS_REGULAR = 0,
    FS_BOLD = 1,
    FS_ITALIC = 2,
} SSFN2FontStyle;

static MaybeError ssfn2_load_internal(IoReader reader)
{
    SSFN2CommonHeader common_header;
    TRY(MaybeError, io_read(reader, (uint8_t *)&common_header, sizeof(SSFN2CommonHeader)));

    if (memcmp(common_header.magic, SSFN_COLLECTION, 4) == 0)
    {
        // uint32_t end = load_le(common_header.size);
        while (ssfn2_load_internal(reader).succ)
            ;
    }
    else if (memcmp(common_header.magic, SSFN_MAGIC, 4) == 0)
    {
        SSFN2FontHeader font_header;
        TRY(MaybeError, io_read(reader, (uint8_t *)&font_header, sizeof(SSFN2FontHeader)));

        SSFN2FontStyle style = load_le(font_header.type);
        UNUSED(style);
    }
    else
    {
        return ERROR(ERR_UNDEFINED);
    }

    return SUCCESS;
}

MaybeError ssfn2_load(IoRSeek rseek)
{
    IoReader reader;
    if (gzip_probe(rseek))
    {
        // TODO:
        // reader = gzip_reader(rseek.reader);
        return ERROR(ERR_NOT_IMPLEMENTED);
    }
    else
    {
        reader = rseek.reader;
    }
    ssfn2_load_internal(reader);

    return SUCCESS;
}
