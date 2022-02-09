#include <brutal/codec/gzip/gzip.h>
#include <brutal/codec/ssfn2/ssfn2.h>
#include <brutal/debug/log.h>
#include <brutal/gfx/gfx.h>

#define SSFN2_MAGIC str$("SFN2")
#define SSFN2_COLLECTION str$("SFNC")
#define SSFN2_ENDMAGIC str$("2NFS")

#define SSFN2_CMD_MOVE_TO 0
#define SSFN2_CMD_LINE_TO 1
#define SSFN2_CMD_QUAD_CURVE 2
#define SSFN2_CMD_BEZIER_CURVE 3

typedef struct PACKED
{
    le_uint8_t magic[4];
    le_uint32_t size; /* total size in bytes */
} SSFN2CommonHeader;

typedef enum
{
    FS_REGULAR = 0,
    FS_BOLD = 1,
    FS_ITALIC = 2,
} SSFN2FontStyle;

static MaybeError ssfn2_load_string(IoReader reader, char *dst)
{
    for (size_t i = 0; i < SSFN2_MAX_STR_LEN; i++)
    {
        uint8_t c;
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
        log$("SSFN2: incorrect fragment offset {}", offset);
        return ERROR(ERR_BAD_ADDRESS);
    }

    size_t prev_offset = TRY(MaybeError, io_tell(rseek.seeker));
    TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(offset)));

    uint8_t val;
    TRY(MaybeError, io_read_byte(rseek.reader, &val));

    if (val & 0x80)
    {
        log$("Non contour fragment not supported!");
        TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(prev_offset)));
        return ERROR(ERR_NOT_IMPLEMENTED);
    }

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

    if (cmd_bytes > 255)
    {
        log$("SSFN2: incorrect fragment command count {}", cmd_count);
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
            TRY(MaybeError, io_read_byte(rseek.reader, &x));
            TRY(MaybeError, io_read_byte(rseek.reader, &y));

            uint8_t cmd = (cmd_data[i] >> (j * 2)) & 0b00000011;

            switch (cmd)
            {
            case SSFN2_CMD_MOVE_TO:
                gfx_path_move_to(path, m_vec2(x, y));
                break;

            case SSFN2_CMD_LINE_TO:
                gfx_path_line_to(path, m_vec2(x, y));
                break;

            case SSFN2_CMD_QUAD_CURVE:
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1x));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1y));

                gfx_path_quadratic_to(path, m_vec2(cp1x, cp1y), m_vec2(x, y));
                break;

            case SSFN2_CMD_BEZIER_CURVE:
                TRY(MaybeError, io_read_byte(rseek.reader, &cp2x));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp2y));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1x));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1y));

                gfx_path_cubic_to(path, m_vec2(cp1x, cp1y), m_vec2(cp2x, cp2y), m_vec2(x, y));
                break;

            default:
                log$("SSFN2: unknown command {}", cmd);
                break;
            }
        }
    }

    TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(prev_offset)));
    return SUCCESS;
}

static MaybeError ssfn2_load_mapping(IoRSeek rseek, SSFN2Font *font, Rune rune, uint8_t attrs, Alloc *alloc)
{
    uint8_t glyph_data[5];
    TRY(MaybeError, io_read(rseek.reader, glyph_data, sizeof(glyph_data)));

    // Large fragments
    uint8_t frag_size = attrs & 0x40 ? 6 : 5;
    uint8_t num_frags = glyph_data[0];

    // Initialize glyph
    font->glyphs[rune] = (SSFN2Glyph){
        .width = glyph_data[1],
        .height = glyph_data[2],
        .adv_x = glyph_data[3],
        .adv_y = glyph_data[4],
    };

    gfx_path_init(&font->glyphs[rune].path, alloc);

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
            {
                return ERROR(ERR_NOT_FOUND);
            }
        }
        else
        {
            size_t frag_off = 0;
            for (size_t idx = frag_size - 1; idx > 1; idx--)
            {
                frag_off = frag_off << 8;
                frag_off |= frag_data[idx];
            }

            ssfn2_load_fragment(rseek, font, frag_off, &font->glyphs[rune].path);
        }
    }

    return SUCCESS;
}

static MaybeError ssfn2_load_mappings(IoRSeek rseek, SSFN2Font *font, SSFN2CommonHeader *common_header, Alloc *alloc)
{
    size_t char_offs = load_le(font->header.characters_offs);
    size_t lig_offs = load_le(font->header.ligature_offs);
    size_t kern_offs = load_le(font->header.kerning_offs);
    size_t size = load_le(common_header->size);

    io_seek(rseek.seeker, io_seek_from_start(char_offs));
    size_t end = lig_offs ? lig_offs : kern_offs ? kern_offs
                                                 : size - 4;

    int rune = 0;
    while (TRY(MaybeError, io_tell(rseek.seeker)) < end)
    {
        uint8_t attrs;
        TRY(MaybeError, io_read_byte(rseek.reader, &attrs));

        if (attrs == 0b11111111)
        {
            // Skip UINT_MAX + 1 runes
            rune += 65356;
        }
        else if ((attrs & 0b11000000) == 0b10000000)
        {
            // Skip up to 64 runes
            rune += (attrs & 0b00111111) + 1;
        }
        else if ((attrs & 0b11000000) == 0b11000000)
        {
            // Skip up to 16128 runes
            uint8_t extra;
            TRY(MaybeError, io_read_byte(rseek.reader, &extra));
            rune += (((attrs & 0b00111111) << 8) | extra) + 1;
        }
        else
        {
            // Get a rune
            TRY(MaybeError, ssfn2_load_mapping(rseek, font, rune, attrs, alloc));
            rune++;
        }
    }

    return SUCCESS;
}

static MaybeError ssfn2_load_internal(IoRSeek rseek, SSFN2Font *font, Alloc *alloc)
{
    IoReader reader = rseek.reader;
    SSFN2CommonHeader common_header;
    TRY(MaybeError, io_read(reader, (uint8_t *)&common_header, sizeof(SSFN2CommonHeader)));

    if (str_eq(str_n$(4, (char const *)common_header.magic), SSFN2_COLLECTION))
    {
        // uint32_t end = load_le(common_header.size);
        while (ssfn2_load_internal(rseek, font, alloc).succ)
            ;
    }
    else if (str_eq(str_n$(4, (char const *)common_header.magic), SSFN2_MAGIC))
    {
        TRY(MaybeError, io_read(reader, (uint8_t *)&font->header, sizeof(SSFN2FontHeader)));
        TRY(MaybeError, ssfn2_load_stringtable(reader, font));

        font->glyphs = alloc_make_array(alloc, SSFN2Glyph, 0x110000);

        TRY(MaybeError, ssfn2_load_mappings(rseek, font, &common_header, alloc));
    }
    else
    {
        return ERROR(ERR_UNDEFINED);
    }

    return SUCCESS;
}

MaybeError ssfn2_load(IoRSeek rseek, SSFN2Font *font, Alloc *alloc)
{
    if (gzip_probe(rseek))
    {
        // TODO:
        // reader = gzip_reader(rseek.reader);
        return ERROR(ERR_NOT_IMPLEMENTED);
    }

    return ssfn2_load_internal(rseek, font, alloc);
}
