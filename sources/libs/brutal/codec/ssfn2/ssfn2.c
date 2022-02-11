#include <brutal/alloc/global.h>
#include <brutal/codec/gzip/gzip.h>
#include <brutal/codec/ssfn2/ssfn2.h>
#include <brutal/debug.h>
#include <brutal/gfx.h>

#define SSFN2_MAGIC str$("SFN2")
#define SSFN2_COLLECTION str$("SFNC")
#define SSFN2_ENDMAGIC str$("2NFS")

/* font family group in font type byte */
#define SSFN2_TYPE_FAMILY(x) ((x)&15)
#define SSFN2_FAMILY_SERIF 0
#define SSFN2_FAMILY_SANS 1
#define SSFN2_FAMILY_DECOR 2
#define SSFN2_FAMILY_MONOSPACE 3
#define SSFN2_FAMILY_HAND 4

/* font style flags in font type byte */
#define SSFN2_TYPE_STYLE(x) (((x) >> 4) & 15)
#define SSFN2_STYLE_REGULAR 0
#define SSFN2_STYLE_BOLD 1
#define SSFN2_STYLE_ITALIC 2
#define SSFN2_STYLE_USRDEF1 4 /* user defined variant 1 */
#define SSFN2_STYLE_USRDEF2 8 /* user defined variant 2 */

enum
{
    SSFN2_CMD_MOVE_TO,
    SSFN2_CMD_LINE_TO,
    SSFN2_CMD_QUAD_CURVE,
    SSFN2_CMD_BEZIER_CURVE,
};

typedef struct PACKED
{
    le_uint8_t magic[4];
    le_uint32_t size; /* total size in bytes */
} SSFN2CommonHeader;

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

static MaybeError ssfn2_load_fragment(IoRSeek rseek, SSFN2Font *font, MVec2 pos, size_t offset, GfxPath *path)
{
    uint32_t fragments_offs = load_le(font->header.fragments_offs);
    uint32_t characters_offs = load_le(font->header.characters_offs);

    if (offset < fragments_offs || offset >= characters_offs)
    {
        log$("SSFN2: incorrect fragment offset {}", offset);
        return ERROR(ERR_BAD_ADDRESS);
    }

    size_t prev_offset = TRY(MaybeError, io_tell(rseek.seeker));
    TRY(MaybeError, io_seek(rseek.seeker, io_seek_from_start(font->font_start + offset)));

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
        TRY(MaybeError, io_read_byte(rseek.reader, &val));

        cmd_count <<= 8;
        cmd_count |= val;
    }
    cmd_count++;

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
    size_t cmd_cur = 0;

    for (size_t i = 0; i < (cmd_count / 4) + 1; i++)
    {
        // 4 commands per byte
        for (size_t j = 0; j < 4 && cmd_cur < cmd_count; j++)
        {
            TRY(MaybeError, io_read_byte(rseek.reader, &x));
            TRY(MaybeError, io_read_byte(rseek.reader, &y));

            uint8_t cmd = (cmd_data[i] >> (j * 2)) & 0b00000011;

            switch (cmd)
            {
            case SSFN2_CMD_MOVE_TO:
                gfx_path_move_to(path, m_vec2_add(pos, m_vec2(x, y)));
                break;

            case SSFN2_CMD_LINE_TO:
                gfx_path_line_to(path, m_vec2_add(pos, m_vec2(x, y)));
                break;

            case SSFN2_CMD_QUAD_CURVE:
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1x));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1y));

                gfx_path_quadratic_to(
                    path,
                    m_vec2_add(pos, m_vec2(cp1x, cp1y)),
                    m_vec2_add(pos, m_vec2(x, y)));
                break;

            case SSFN2_CMD_BEZIER_CURVE:
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1x));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp1y));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp2x));
                TRY(MaybeError, io_read_byte(rseek.reader, &cp2y));

                gfx_path_cubic_to(
                    path,
                    m_vec2_add(pos, m_vec2(cp1x, cp1y)),
                    m_vec2_add(pos, m_vec2(cp2x, cp2y)),
                    m_vec2_add(pos, m_vec2(x, y)));
                break;

            default:
                log$("SSFN2: unknown command {}", cmd);
                break;
            }
            cmd_cur++;
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

            ssfn2_load_fragment(rseek, font, m_vec2(x, y), frag_off, &font->glyphs[rune].path);
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

    io_seek(rseek.seeker, io_seek_from_start(font->font_start + char_offs));
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
            if (rune == 'B')
            {
                gfx_path_dump(&font->glyphs[rune].path);
            }
            rune++;
        }
    }

    return SUCCESS;
}

static MaybeError ssfn2_load_internal(IoRSeek rseek, SSFN2Collection *collection, Alloc *alloc)
{
    IoReader reader = rseek.reader;
    IoSeeker seeker = rseek.seeker;

    size_t start = TRY(MaybeError, io_tell(seeker));
    SSFN2CommonHeader common_header;
    TRY(MaybeError, io_read(reader, (uint8_t *)&common_header, sizeof(SSFN2CommonHeader)));
    size_t size = load_le(common_header.size);

    if (str_eq(str_n$(4, (char const *)common_header.magic), SSFN2_COLLECTION))
    {
        // uint32_t end = load_le(common_header.size);
        while (TRY(MaybeError, io_tell(seeker)) != start + size)
        {
            TRY(MaybeError, ssfn2_load_internal(rseek, collection, alloc));
        }
    }
    else if (str_eq(str_n$(4, (char const *)common_header.magic), SSFN2_MAGIC))
    {
        SSFN2Font font;
        font.font_start = start;
        TRY(MaybeError, io_read(reader, (uint8_t *)&font.header, sizeof(SSFN2FontHeader)));
        TRY(MaybeError, ssfn2_load_stringtable(reader, &font));

        font.glyphs = alloc_make_array(alloc, SSFN2Glyph, 0x110000);

        TRY(MaybeError, ssfn2_load_mappings(rseek, &font, &common_header, alloc));
        vec_push(collection, font);

        // Seek to the end to verify the end magic
        le_uint8_t end_magic[4];
        TRY(MaybeError, io_seek(seeker, io_seek_from_start(font.font_start + size - 4)));
        TRY(MaybeError, io_read(reader, (uint8_t *)&end_magic, 4));
        if (!str_eq(str_n$(4, (char const *)end_magic), SSFN2_ENDMAGIC))
        {
            return ERROR(ERR_UNDEFINED);
        }
    }
    else
    {
        return ERROR(ERR_UNDEFINED);
    }

    return SUCCESS;
}

MaybeError ssfn2_load(IoRSeek rseek, SSFN2Collection *collection, Alloc *alloc)
{
    vec_init(collection, alloc_global());

    IoRSeek input = rseek;
    Buf buf;

    if (gzip_probe(rseek))
    {
        buf_init(&buf, 1024, alloc_global());
        IoWriter writer = buf_writer(&buf);
        TRY(MaybeError, gzip_decompress_stream(writer, rseek.reader));
        input = buf_rseek(&buf);
    }

    return ssfn2_load_internal(input, collection, alloc);
}

/* --- SSFN2 Font --------------------------------------------------------- */

Str gfx_font_style_subfamily(GfxFontStyle style)
{
    Str result = str$("");
    // Weights
    if (style.weight == GFX_FONT_EXTRA_BOLD)
        result = str$("ExtraBold");
    else if (style.weight == GFX_FONT_BOLD)
        result = str$("Bold");
    else if (style.weight == GFX_FONT_SEMI_BOLD)
        result = str$("SemiBold");
    else if (style.weight == GFX_FONT_MEDIUM)
        result = str$("Medium");
    else if (style.weight == GFX_FONT_REGULAR)
        result = str$("Regular");
    else if (style.weight == GFX_FONT_LIGHT)
        result = str$("Light");
    else if (style.weight == GFX_FONT_EXTRA_LIGHT)
        result = str$("ExtraLight");
    else if (style.weight == GFX_FONT_THIN)
        result = str$("Thin");

    // Italic
    if (style.italic)
        str_concat(result, str$("Italic"), alloc_global());

    return result;
}

SSFN2Font *gfx_font_ssfn2_select(void *ctx, GfxFontStyle style)
{
    SSFN2Collection *coll = (SSFN2Collection *)ctx;
    
    // Check if any font is an exact match
    Str wanted_sub = gfx_font_style_subfamily(style);
    vec_foreach(font, coll)
    {
        if (str_eq(str$(font->stringtable.subfamily_name), wanted_sub))
            return font;
    }

    // Fallback heuristic
    SSFN2Font *best_match = NULL;
    uint8_t best_score = 0;

    vec_foreach(font, coll)
    {
        uint8_t score = 1;
        uint8_t type = load_le(font->header.type);

        if (style.italic && (SSFN2_TYPE_STYLE(type) & SSFN2_STYLE_ITALIC))
        {
            score++;
        }
        if (style.weight >= GFX_FONT_BOLD && (SSFN2_TYPE_STYLE(type) & SSFN2_STYLE_BOLD))
        {
            score++;
        }
        if (style.weight == GFX_FONT_REGULAR && (SSFN2_TYPE_STYLE(type) & SSFN2_STYLE_REGULAR))
        {
            score++;
        }

        if (score > best_score)
        {
            best_match = font;
            best_score = score;
        }
    }

    return best_match;
}

GfxFontMetrics gfx_font_ssfn2_metrics(void *ctx, GfxFontStyle style)
{
    SSFN2Font *font = gfx_font_ssfn2_select(ctx, style);

    return (GfxFontMetrics){
        .line_ascend = 12 * style.scale,
        .ascend = 10 * style.scale,
        .captop = 10 * style.scale,
        .descend = 3 * style.scale,
        .line_descend = 4 * style.scale,

        .advance = load_le(font->header.width) * style.scale,
    };
}

float gfx_font_ssfn2_advance(void *ctx, GfxFontStyle style, Rune rune)
{
    SSFN2Font *font = gfx_font_ssfn2_select(ctx, style);
    return font->glyphs[rune].width;
}

void gfx_font_ssfn2_render(void *ctx, GfxFontStyle style, Gfx *gfx, MVec2 baseline, Rune rune)
{
    gfx_push(gfx);
    SSFN2Font *font = gfx_font_ssfn2_select(ctx, style);
    SSFN2Glyph glyph = font->glyphs[rune];
    gfx_origin(gfx, baseline);
    gfx_stroke_path(gfx, &glyph.path);

    gfx_pop(gfx);
}

GfxFont gfx_font_ssfn2(SSFN2Collection *coll)
{
    return (GfxFont){
        .ctx = coll,
        .style = {
            .scale = 0.01,
            .weight = GFX_FONT_REGULAR,
            .italic = false,
        },
        .metrics = gfx_font_ssfn2_metrics,
        .advance = gfx_font_ssfn2_advance,
        .render = gfx_font_ssfn2_render,
    };
}
