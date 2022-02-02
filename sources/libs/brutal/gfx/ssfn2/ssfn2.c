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

static MaybeError ssfn2_load_mappings(IoRSeek rseek, SSFN2Font *font, SSFN2CommonHeader *common_header)
{
    size_t char_offs = load_le(font->header.characters_offs);
    size_t lig_offs = load_le(font->header.ligature_offs);
    size_t size = load_le(common_header->size);

    io_seek(rseek.seeker, io_seek_from_start(char_offs));
    size_t end = lig_offs ? lig_offs : size - 4;

    int unicode = 0;
    while (TRY(MaybeError, io_tell(rseek.seeker)) < end)
    {
        uint8_t val;
        TRY(MaybeError, io_read_byte(rseek.reader, &val));

        // Skip UINT_MAX + 1 codepoints
        if (val == 0b11111111)
        {
            unicode += 65356;
        }
        // Skip up to 64 codepoints
        else if ((val & 0b11000000) == 0b10000000)
        {
            unicode += val & 0b00111111;
        }
        // Skip up to 16128 codepoints
        else if ((val & 0b11000000) == 0b11000000)
        {
            uint8_t extra;
            TRY(MaybeError, io_read_byte(rseek.reader, &extra));
            unicode += ((val & 0b00111111) << 8) | extra;
        }
        // Get a codepoint
        else
        {
            uint8_t glyph_data[5];
            TRY(MaybeError, io_read(rseek.reader, glyph_data, sizeof(glyph_data)));

            uint8_t attributes = val;
            // Large fragments
            uint8_t frag_size = attributes & 0x40 ? 6 : 5;
            uint8_t num_frags = glyph_data[0];
            font->glyphs[unicode] = (SSFN2Glyph){
                .width = glyph_data[1],
                .height = glyph_data[2],
                .adv_x = glyph_data[3],
                .adv_y = glyph_data[4],
            };
            uint8_t extra_bytes = val;
            uint8_t extra_data[0b01111111];
            TRY(MaybeError, io_read(rseek.reader, extra_data, extra_bytes));

            for (size_t frag = 0; frag < num_frags; frag++)
            {
                size_t data_off = frag * frag_size;
                uint8_t x = extra_data[data_off];
                uint8_t y = extra_data[data_off + 1];
                // This is a color
                if (x == 0xFF && y == 0xFF)
                {
                    uint8_t color_idx = extra_data[data_off + 2];
                    // Invalid color index. See
                    if (color_idx > 0xFD)
                        return ERROR(ERR_NOT_FOUND);
                }
                else
                {
                    uint8_t frag_off = 0;
                    for (size_t idx = 2; idx < frag_size; idx++)
                    {
                        frag_off = frag_off << 8;
                        frag_off |= extra_data[data_off + idx];
                    }
                }
            }
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
        TRY(MaybeError, ssfn2_load_mappings(rseek, font, &common_header));
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
    UNUSED(reader);

    return ssfn2_load_internal(rseek, font);
}
