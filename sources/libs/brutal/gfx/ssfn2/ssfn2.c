#include <brutal/alloc/global.h>
#include <brutal/codec/gzip/gzip.h>
#include <brutal/debug/log.h>
#include <brutal/gfx/gfx.h>
#include <brutal/gfx/ssfn2/ssfn2.h>
#include <string.h>

/* magic bytes */
#define SSFN2_MAGIC "SFN2"
#define SSFN2_COLLECTION "SFNC"
#define SSFN2_ENDMAGIC "2NFS"

#define SSFN2_CMD_MOVE_TO 0
#define SSFN2_CMD_LINE_TO 1
#define SSFN2_CMD_QUAD_CURVE 2
#define SSFN2_CMD_BEZIER_CURVE 3

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

static MaybeError ssfn2_load_fragment(IoRSeek rseek, SSFN2Font *font, size_t offset, GfxPath *path)
{
    uint32_t fragments_offs = load_le(font->header.fragments_offs);
    uint32_t characters_offs = load_le(font->header.characters_offs);

    if (offset < fragments_offs || offset >= characters_offs)
    {
        log$("SSFN2: incorrect fragment offset {}\n", offset);
        return ERROR(ERR_BAD_ADDRESS);
    }

    size_t prev_offset = TRY(MaybeError, io_tell(rseek.seeker));
    TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(offset)));

    uint8_t val;
    TRY(MaybeError, io_read_byte(rseek.reader, &val));

    if (!(val & 0x80))
    {
        size_t cmd_count = val & 0b00111111;
        // contour with more commands (1 extra byte)
        if (val & 0b01000000)
        {
            cmd_count <<= 8;
            TRY(MaybeError, io_read_byte(rseek.reader, &val));
            cmd_count += val;
        }

        // Read all command bytes
        size_t cmd_bytes = (cmd_count + 3) / 4;

        if (cmd_bytes > 255){
            log$("SSFN2: incorrect fragment command count {}\n", cmd_count);
            return ERROR(ERR_BAD_ADDRESS);
        }

        uint8_t cmd_data[0b111111111111];
        TRY(MaybeError, io_read(rseek.reader, cmd_data, cmd_bytes));

        uint8_t x, y, cp1x, cp1y, cp2x, cp2y;
        for (size_t i = 0; i < cmd_count; i++)
        {
            // 4 commands per byte
            for (size_t j = 0; j < 4; j++)
            {
                GfxPathCmd cmd = {};

                // All commands require X & Y
                TRY(MaybeError, io_read_byte(rseek.reader, &x));
                TRY(MaybeError, io_read_byte(rseek.reader, &y));

                uint8_t ssfn2_cmd = (cmd_data[i] >> (j * 2)) & 0b00000011;

                switch (ssfn2_cmd)
                {
                case SSFN2_CMD_MOVE_TO:
                    cmd.type = GFX_CMD_MOVE_TO;
                    cmd.point = (MVec2){.x = x, .y = y};
                    break;

                case SSFN2_CMD_LINE_TO:
                    cmd.type = GFX_CMD_LINE_TO;
                    cmd.point = (MVec2){.x = x, .y = y};
                    break;

                case SSFN2_CMD_QUAD_CURVE:
                    cmd.type = GFX_CMD_QUADRATIC_TO;
                    cmd.point = (MVec2){.x = x, .y = y};
                    TRY(MaybeError, io_read_byte(rseek.reader, &cp1x));
                    TRY(MaybeError, io_read_byte(rseek.reader, &cp1y));
                    cmd.cp = (MVec2){.x = cp1x, .y = cp1y};
                    break;

                case SSFN2_CMD_BEZIER_CURVE:
                    cmd.type = GFX_CMD_CUBIC_TO;
                    cmd.point = (MVec2){.x = x, .y = y};
                    TRY(MaybeError, io_read_byte(rseek.reader, &cp1x));
                    TRY(MaybeError, io_read_byte(rseek.reader, &cp1y));
                    cmd.cp1 = (MVec2){.x = cp1x, .y = cp1y};
                    TRY(MaybeError, io_read_byte(rseek.reader, &cp2x));
                    TRY(MaybeError, io_read_byte(rseek.reader, &cp2y));
                    cmd.cp2 = (MVec2){.x = cp2x, .y = cp2y};
                    break;

                default:
                    break;
                }

                vec_push(path, cmd);
            }
        }
    }
    else
    {
        log$("Non contour fragment not supported!");
        TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(prev_offset)));
        return ERROR(ERR_NOT_IMPLEMENTED);
    }

    TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(prev_offset)));
    return SUCCESS;
}

static MaybeError ssfn2_load_mappings(IoRSeek rseek, SSFN2Font *font, SSFN2CommonHeader *common_header)
{
    size_t char_offs = load_le(font->header.characters_offs);
    size_t lig_offs = load_le(font->header.ligature_offs);
    size_t kern_offs = load_le(font->header.kerning_offs);
    size_t size = load_le(common_header->size);

    io_seek(rseek.seeker, io_seek_from_start(char_offs));
    size_t end = lig_offs ? lig_offs : kern_offs ? kern_offs : size - 4;

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

            // Initialize glyph
            font->glyphs[unicode] = (SSFN2Glyph){
                .width = glyph_data[1],
                .height = glyph_data[2],
                .adv_x = glyph_data[3],
                .adv_y = glyph_data[4],
            };

            vec_init(&font->glyphs[unicode].path, alloc_global());

            for (size_t frag = 0; frag < num_frags; frag++)
            {
                uint8_t frag_data[6];
                TRY(MaybeError, io_read(rseek.reader, frag_data, frag_size));

                uint8_t x = frag_data[0];
                uint8_t y = frag_data[1];
                // This is a color
                if (x == 0xFF && y == 0xFF)
                {
                    uint8_t color_idx = frag_data[2];
                    // Invalid color index. See https://gitlab.com/bztsrc/scalable-font2/-/blob/master/docs/sfn_format.md#fragment-descriptors
                    if (color_idx > 0xFD)
                        return ERROR(ERR_NOT_FOUND);
                }
                else
                {
                    size_t frag_off = 0;
                    for (size_t idx = frag_size - 1; idx > 1; idx--)
                    {
                        frag_off = frag_off << 8;
                        frag_off |= frag_data[idx];
                    }

                    ssfn2_load_fragment(rseek, font, frag_off, &font->glyphs[unicode].path);
                }
            }

            unicode++;
        }
    }

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

        font->glyphs = alloc_make_array(alloc_global(), SSFN2Glyph, 0x110000);

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

void font_ssfn2_deinit(SSFN2Font *font)
{
    alloc_free(alloc_global(), font->glyphs);
}
