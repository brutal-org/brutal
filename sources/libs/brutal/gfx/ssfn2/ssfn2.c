#include <brutal/codec/gzip/gzip.h>
#include <brutal/gfx/ssfn2/ssfn2.h>
#include <string.h>

/* magic bytes */
#define SSFN2_MAGIC "SFN2"
#define SSFN2_COLLECTION "SFNC"
#define SSFN2_ENDMAGIC "2NFS"

typedef struct PACKED
{
    le_uint8_t magic[4]; /* SSFN magic bytes */
    le_uint32_t size;    /* total size in bytes */
} SSFN2CommonHeader;

typedef enum
{
    FS_REGULAR = 0,
    FS_BOLD = 1,
    FS_ITALIC = 2,
} SSFN2FontStyle;

static MaybeError ssfn2_load_string(IoReader reader, char *dst)
{
    mem_set(dst, 0, SSFN2_MAX_STR_LEN);
    uint8_t c = '\0';
    for (size_t i = 0; i < SSFN2_MAX_STR_LEN; i++)
    {
        TRY(MaybeError, io_read_byte(reader, &c));
        dst[i] = c;
        if (c == '\0')
        {
            break;
        }
    }
    return SUCCESS;
}

static MaybeError ssfn2_load_mappings(IoRSeek rseek, SSFN2FontHeader *header, SSFN2CommonHeader *common_header)
{
    size_t char_offs = load_le(header->characters_offs);
    size_t lig_offs = load_le(header->ligature_offs);
    size_t size = load_le(common_header->size);

    io_seek(rseek.seeker, io_seek_from_start(char_offs));
    size_t end = lig_offs ? lig_offs : size - 4;

    int unicode = 0;
    for (size_t unicode = 0; UNWRAP(io_tell(rseek.seeker)) < end;)
    {
        uint8_t val;
        TRY(MaybeError, io_read_byte(rseek.reader, &val));

        if(val == 0b11111111)
        {
            unicode += 65356;
        }
        else if(val & 0b11000000)
        {
            
        }
    }

    return SUCCESS;
}

static MaybeError ssfn2_load_stringtable(IoReader reader, SSFN2Font *font)
{
    TRY(MaybeError, ssfn2_load_string(reader, font->stringtable.font_name));
    TRY(MaybeError, ssfn2_load_string(reader, font->stringtable.family_name));
    TRY(MaybeError, ssfn2_load_string(reader, font->stringtable.subfamily_name));
    TRY(MaybeError, ssfn2_load_string(reader, font->stringtable.revision));
    TRY(MaybeError, ssfn2_load_string(reader, font->stringtable.manufacturer));
    TRY(MaybeError, ssfn2_load_string(reader, font->stringtable.license));
    return SUCCESS;
}

static MaybeError ssfn2_load_internal(IoRSeek rseek, SSFN2Font *font)
{
    IoReader reader = rseek.reader;
    SSFN2CommonHeader common_header;
    TRY(MaybeError, io_read(reader, (uint8_t *)&common_header, sizeof(SSFN2CommonHeader)));

    if (memcmp(common_header.magic, SSFN2_COLLECTION, 4) == 0)
    {
        // uint32_t end = load_le(common_header.size);
        while (ssfn2_load_internal(rseek, font).succ)
            ;
    }
    else if (memcmp(common_header.magic, SSFN2_MAGIC, 4) == 0)
    {
        TRY(MaybeError, io_read(reader, (uint8_t *)&font->header, sizeof(SSFN2FontHeader)));
        TRY(MaybeError, ssfn2_load_stringtable(reader, font));
        TRY(MaybeError, ssfn2_load_mappings(rseek, &font->header, &common_header));
    }
    else
    {
        return ERROR(ERR_UNDEFINED);
    }

    return SUCCESS;
}

MaybeError font_ssfn2_init(IoRSeek rseek, SSFN2Font *font)
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

    return ssfn2_load_internal(reader, font);
}
