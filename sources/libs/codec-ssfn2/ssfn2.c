#include <brutal-debug>
#include <brutal-gfx>
#include <codec-gzip>

#include "ssfn2.h"

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

static MaybeError ssfn2_read_point(IoRSeek rseek, SSFN2FontHeader *header, MVec2f pos, MVec2f *result)
{
    uint8_t x, y;
    TRY(MaybeError, io_read_byte$(rseek, &x));
    TRY(MaybeError, io_read_byte$(rseek, &y));

    MVec2f point = m_vec2f_add(pos, m_vec2f(x, y));
    point = m_vec2f_sub(point, m_vec2f(0, le_load$(header->baseline)));
    point = m_vec2f_div_v(point, le_load$(header->baseline));
    point = m_vec2f_mul_v(point, 16);

    *result = point;
    return SUCCESS;
}

static MaybeError ssfn2_load_fragment(IoRSeek rseek, SSFN2Font *font, MVec2f pos, size_t offset, GfxPath *path)
{
    // Add font start offset
    offset += font->font_start;
    uint32_t fragments_offs = font->font_start + le_load$(font->header.fragments_offs);
    uint32_t characters_offs = font->font_start + le_load$(font->header.characters_offs);

    if (offset < fragments_offs || offset >= characters_offs)
    {
        log$("SSFN2: incorrect fragment offset {}", offset);
        return ERROR(ERR_BAD_ADDRESS);
    }

    size_t prev_offset = TRY(MaybeError, io_tell$(rseek));
    TRY(MaybeError, io_seek$(rseek, io_seek_from_start(offset)));

    uint8_t val;
    TRY(MaybeError, io_read_byte$(rseek, &val));

    if (val & 0x80)
    {
        log$("Non contour fragment not supported!");
        TRY(MaybeError, io_seek$(rseek, io_seek_from_start(prev_offset)));
        return ERROR(ERR_NOT_IMPLEMENTED);
    }

    size_t cmd_count = val & 0b00111111;
    // contour with more commands (1 extra byte)
    if (val & 0b01000000)
    {
        TRY(MaybeError, io_read_byte$(rseek, &val));

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
    TRY(MaybeError, io_read$(rseek, cmd_data, cmd_bytes));

    MVec2f point, cp1, cp2;
    size_t cmd_cur = 0;

    for (size_t i = 0; i < (cmd_count / 4) + 1; i++)
    {
        // 4 commands per byte
        for (size_t j = 0; j < 4 && cmd_cur < cmd_count; j++)
        {
            TRY(MaybeError, ssfn2_read_point(rseek, &font->header, pos, &point));

            uint8_t cmd = (cmd_data[i] >> (j * 2)) & 0b00000011;

            switch (cmd)
            {
            case SSFN2_CMD_MOVE_TO:
                gfx_path_move_to(path, point);
                break;

            case SSFN2_CMD_LINE_TO:
                gfx_path_line_to(path, point);
                break;

            case SSFN2_CMD_QUAD_CURVE:
                TRY(MaybeError, ssfn2_read_point(rseek, &font->header, pos, &cp1));

                gfx_path_quadratic_to(path, cp1, point);
                break;

            case SSFN2_CMD_BEZIER_CURVE:
                TRY(MaybeError, ssfn2_read_point(rseek, &font->header, pos, &cp1));
                TRY(MaybeError, ssfn2_read_point(rseek, &font->header, pos, &cp2));

                gfx_path_cubic_to(path, cp1, cp2, point);
                break;

            default:
                log$("SSFN2: unknown command {}", cmd);
                break;
            }
            cmd_cur++;
        }
    }

    TRY(MaybeError, io_seek$(rseek, io_seek_from_start(prev_offset)));
    return SUCCESS;
}

static MaybeError ssfn2_load_mapping(IoRSeek rseek, SSFN2Font *font, Rune rune, uint8_t attrs, Alloc *alloc)
{
    uint8_t glyph_data[5];
    TRY(MaybeError, io_read$(rseek, glyph_data, sizeof(glyph_data)));

    // Large fragments
    uint8_t frag_size = attrs & 0x40 ? 6 : 5;
    uint8_t num_frags = glyph_data[0];

    if (rune >= 0x110000)
    {
        log$("SSFN2: rune out of range {x}", rune);
        return ERROR(ERR_BAD_ADDRESS);
    }

    SSFN2FontHeader const *header = &font->header;

    // Initialize glyph
    font->glyphs[rune] = (SSFN2Glyph){
        .present = true,
        .bound = m_vec2f((glyph_data[1] / (float)le_load$(header->baseline)) * 16.0f,
                         (glyph_data[2] / (float)le_load$(header->baseline) * 16.0f)),

        .advance = m_vec2f((glyph_data[3] / (float)le_load$(header->baseline)) * 16.0f,
                           (glyph_data[4] / (float)le_load$(header->baseline) * 16.0f)),
    };

    gfx_path_init(&font->glyphs[rune].path, alloc);

    for (size_t frag = 0; frag < num_frags; frag++)
    {
        uint8_t frag_data[6];
        TRY(MaybeError, io_read$(rseek, frag_data, frag_size));

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

            ssfn2_load_fragment(rseek, font, m_vec2f(x, y), frag_off, &font->glyphs[rune].path);
        }
    }

    return SUCCESS;
}

static MaybeError ssfn2_load_mappings(IoRSeek rseek, SSFN2Font *font, SSFN2CommonHeader *common_header, Alloc *alloc)
{
    size_t char_offs = le_load$(font->header.characters_offs);
    size_t lig_offs = le_load$(font->header.ligature_offs);
    size_t kern_offs = le_load$(font->header.kerning_offs);
    size_t size = le_load$(common_header->size);

    size_t end = lig_offs ? lig_offs : kern_offs ? kern_offs
                                                 : size - 4;

    char_offs += font->font_start;
    lig_offs += font->font_start;
    kern_offs += font->font_start;
    size += font->font_start;
    end += font->font_start;

    io_seek$(rseek, io_seek_from_start(char_offs));

    int rune = 0;
    while (TRY(MaybeError, io_tell$(rseek)) < end)
    {
        uint8_t attrs;
        TRY(MaybeError, io_read_byte$(rseek, &attrs));

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
            TRY(MaybeError, io_read_byte$(rseek, &extra));
            rune += (((attrs & 0b00111111) << 8) | extra) + 1;
        }
        else
        {
            // Get a rune
            TRY(MaybeError, ssfn2_load_mapping(rseek, font, rune, attrs, alloc));
            rune++;
        }
    }

    if (rune == 0)
    {
        log$("SSFN2: didn't load any runes");
        return ERROR(ERR_UNDEFINED);
    }

    return SUCCESS;
}

static void ssfn2_compute_weight(SSFN2Font *font)
{
    font->weight = GFX_FONT_REGULAR;

    Str subfamily = str$(font->stringtable.subfamily_name);
    // Weights
    if (str_first(subfamily, str$("Black")) >= 0)
        font->weight = GFX_FONT_BLACK;
    else if (str_first(subfamily, str$("Extra Bold")) >= 0)
        font->weight = GFX_FONT_EXTRA_BOLD;
    else if (str_first(subfamily, str$("Semi Bold")) >= 0)
        font->weight = GFX_FONT_SEMI_BOLD;
    else if (str_first(subfamily, str$("Bold")) >= 0)
        font->weight = GFX_FONT_BOLD;
    else if (str_first(subfamily, str$("Medium")) >= 0)
        font->weight = GFX_FONT_MEDIUM;
    else if (str_first(subfamily, str$("Regular")) >= 0)
        font->weight = GFX_FONT_REGULAR;
    else if (str_first(subfamily, str$("Extra Light")) >= 0)
        font->weight = GFX_FONT_EXTRA_LIGHT;
    else if (str_first(subfamily, str$("Light")) >= 0)
        font->weight = GFX_FONT_LIGHT;
    else if (str_first(subfamily, str$("Thin")) >= 0)
        font->weight = GFX_FONT_THIN;
    else
        log$("Found no match for subfamily: {}", subfamily);
}

static MaybeError ssfn2_load_internal(IoRSeek rseek, SSFN2Collection *collection, Alloc *alloc)
{

    size_t start = TRY(MaybeError, io_tell$(rseek));
    SSFN2CommonHeader common_header;
    TRY(MaybeError, io_read$(rseek, (uint8_t *)&common_header, sizeof(SSFN2CommonHeader)));
    size_t size = le_load$(common_header.size);

    if (str_eq(str_n$(4, (char const *)common_header.magic), SSFN2_COLLECTION))
    {
        while (TRY(MaybeError, io_tell$(rseek)) != start + size)
        {
            TRY(MaybeError, ssfn2_load_internal(rseek, collection, alloc));
        }
    }
    else if (str_eq(str_n$(4, (char const *)common_header.magic), SSFN2_MAGIC))
    {
        SSFN2Font font;
        font.font_start = start;
        TRY(MaybeError, io_read$(rseek, (uint8_t *)&font.header, sizeof(SSFN2FontHeader)));
        TRY(MaybeError, ssfn2_load_stringtable(io_reader$(rseek), &font));

        font.glyphs = alloc_make_array(alloc, SSFN2Glyph, 0x110000);

        TRY(MaybeError, ssfn2_load_mappings(rseek, &font, &common_header, alloc));

        // Seek to the end to verify the end magic
        le_uint8_t end_magic[4];
        TRY(MaybeError, io_seek$(rseek, io_seek_from_start(font.font_start + size - 4)));
        TRY(MaybeError, io_read$(rseek, (uint8_t *)&end_magic, 4));
        if (!str_eq(str_n$(4, (char const *)end_magic), SSFN2_ENDMAGIC))
        {
            return ERROR(ERR_UNDEFINED);
        }

        ssfn2_compute_weight(&font);
        vec_push(collection, font);
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

    if (gzip_probe(rseek))
    {
        Buf buf;
        buf_init(&buf, 1024, alloc_global());
        IoWriter writer = buf_writer(&buf);
        TRY(MaybeError, gzip_decompress_stream(writer, io_reader$(rseek)));
        input = buf_rseek(&buf);
    }

    return ssfn2_load_internal(input, collection, alloc);
}

/* --- SSFN2 Font ----------------------------------------------------------- */

SSFN2Font *gfx_font_ssfn2_select(void *ctx, GfxFontStyle style)
{
    SSFN2Collection *coll = (SSFN2Collection *)ctx;

    SSFN2Font *candiate = NULL;
    uint8_t best_match = 0;

    // Get the best match
    vec_foreach(font, coll)
    {
        uint8_t match = 1;

        if (style.weight == font->weight)
            match++;

        uint8_t type = le_load$(font->header.type);
        if (style.italic == ((SSFN2_TYPE_STYLE(type) & SSFN2_STYLE_ITALIC) > 0))
            match++;

        if (match > best_match)
        {
            best_match = match;
            candiate = font;
        }
    }

    assert_truth(candiate);

    return candiate;
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
        .advance = le_load$(font->header.width) * style.scale,
    };
}

float gfx_font_ssfn2_advance(void *ctx, GfxFontStyle style, Rune rune)
{
    SSFN2Font *font = gfx_font_ssfn2_select(ctx, style);
    return font->glyphs[rune].advance.x * style.scale;
}

void gfx_font_ssfn2_render(void *ctx, GfxFontStyle style, Gfx *gfx, MVec2f baseline, Rune rune)
{
    gfx_push(gfx);
    SSFN2Font *font = gfx_font_ssfn2_select(ctx, style);
    SSFN2Glyph glyph = font->glyphs[rune];
    gfx_origin(gfx, baseline);
    gfx_trans(gfx, m_trans2_scale(style.scale));
    gfx_fill_path(gfx, &glyph.path, GFX_FILL_EVENODD);

    gfx_pop(gfx);
}

GfxFont ssfn2_font(SSFN2Collection *coll)
{
    return (GfxFont){
        .ctx = coll,
        .metrics = gfx_font_ssfn2_metrics,
        .advance = gfx_font_ssfn2_advance,
        .render = gfx_font_ssfn2_render,
    };
}
