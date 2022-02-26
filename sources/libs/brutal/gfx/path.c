#include <brutal/debug/locked.h>
#include <brutal/gfx/path.h>
#include <brutal/math/funcs.h>
#include <brutal/math/trans2.h>
#include <brutal/parse/nums.h>

/* --- Path Object ---------------------------------------------------------- */

void gfx_path_init(GfxPath *path, Alloc *alloc)
{
    vec_init(path, alloc);
}

void gfx_path_deinit(GfxPath *path)
{
    vec_deinit(path);
}

void gfx_path_dump(GfxPath *path)
{
    vec_foreach_v(cmd, path)
    {
        switch (cmd.type)
        {
        default:
            log$("bad");
            break;

        case GFX_CMD_NOP:
            log$("nop");
            break;

        case GFX_CMD_MOVE_TO:
            log$("move-to ({}, {})", cmd.point.x, cmd.point.x);
            break;

        case GFX_CMD_CLOSE_PATH:
            log$("close-path");
            break;

        case GFX_CMD_LINE_TO:
            log$("line-to ({}, {})", cmd.point.x, cmd.point.x);
            break;

        case GFX_CMD_CUBIC_TO:
            log$("cubic-to ({}, {})", cmd.point.x, cmd.point.x);
            break;

        case GFX_CMD_QUADRATIC_TO:
            log$("quadratic-to ({}, {})", cmd.point.x, cmd.point.x);
            break;

        case GFX_CMD_ARC_TO:
            log$("arc-to ({}, {})", cmd.point.x, cmd.point.x);
            break;
        }
    }
}

void gfx_path_move_to(GfxPath *path, MVec2 p)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_MOVE_TO,
        .point = p,
    };

    vec_push(path, cmd);
}

void gfx_path_close(GfxPath *path)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_CLOSE_PATH,
    };

    vec_push(path, cmd);
}

void gfx_path_line_to(GfxPath *path, MVec2 p)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_LINE_TO,
        .point = p,
    };

    vec_push(path, cmd);
}

void gfx_path_cubic_to(GfxPath *path, MVec2 cp1, MVec2 cp2, MVec2 p)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_CUBIC_TO,
        .cp1 = cp1,
        .cp2 = cp2,
        .point = p,
    };

    vec_push(path, cmd);
}

void gfx_path_quadratic_to(GfxPath *path, MVec2 cp, MVec2 p)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_QUADRATIC_TO,
        .cp = cp,
        .point = p,
    };

    vec_push(path, cmd);
}

void gfx_path_arc_to(GfxPath *path, float rx, float ry, float angle, int flags, MVec2 p)
{
    GfxPathCmd cmd = {
        .type = GFX_CMD_ARC_TO,
        .rx = rx,
        .ry = ry,
        .angle = angle,
        .flags = flags,
        .point = p,
    };

    vec_push(path, cmd);
}

/* --- Parser --------------------------------------------------------------- */

static void parse_whitespace(Scan *scan)
{
    scan_eat_any(scan, str$(GFX_PATH_WS));
}

static bool parse_whitespace_or_comma(Scan *scan)
{
    parse_whitespace(scan);

    if (scan_skip(scan, ','))
    {
        parse_whitespace(scan);
        return true;
    }

    return false;
}

static float parse_float(Scan *scan)
{
    double result = 0;
    scan_next_double(scan, &result);
    return result;
}

static MVec2 parse_coordinate(Scan *scan)
{
    MVec2 result = {};
    result.x = parse_float(scan);
    parse_whitespace_or_comma(scan);
    result.y = parse_float(scan);
    return result;
}

static int parse_arcflags(Scan *scan)
{
    int flags = 0;

    if (scan_next(scan) == '1')
    {
        flags |= GFX_PATH_LARGE;
    }

    parse_whitespace_or_comma(scan);

    if (scan_next(scan) == '1')
    {
        flags |= GFX_PATH_SWEEP;
    }

    return flags;
}

static MVec2 eval_reflected_cp(GfxPathParser *self)
{
    return m_vec2_sub(m_vec2_mul_v(self->last, 2), self->cp);
}

static MVec2 eval_point(GfxPathParser *self, bool rel, MVec2 point)
{
    if (rel)
    {
        return m_vec2_add(self->last, point);
    }
    else
    {
        return point;
    }
}

static void eval_cmd(GfxPathParser *self, GfxPathCmd cmd)
{
    cmd.cp1 = eval_point(self, cmd.rel, cmd.cp1);
    cmd.cp2 = eval_point(self, cmd.rel, cmd.cp2);
    cmd.point = eval_point(self, cmd.rel, cmd.point);

    if (cmd.type == GFX_CMD_MOVE_TO)
    {
        self->start = cmd.point;
    }

    self->cp = cmd.cp;
    self->last = cmd.point;
    cmd.rel = false;

    self->eval(self->ctx, cmd);
}

static void parse_cmd(GfxPathParser *self, Scan *scan, char c)
{
    GfxPathCmd cmd = {};
    cmd.rel = islower(c);
    c = tolower(c);

    switch (c)
    {
    case 'm':
        cmd.type = GFX_CMD_MOVE_TO;
        cmd.point = parse_coordinate(scan);
        cmd.cp = cmd.point;
        break;

    case 'z':
        cmd.type = GFX_CMD_CLOSE_PATH;
        break;

    case 'l':
        cmd.type = GFX_CMD_LINE_TO;
        cmd.point = parse_coordinate(scan);
        cmd.cp = cmd.point;
        break;

    case 'h':
    {
        cmd.type = GFX_CMD_LINE_TO;
        cmd.point = m_vec2(parse_float(scan), cmd.rel ? 0 : self->last.y);
        cmd.cp = cmd.point;
        break;
    }

    case 'v':
        cmd.type = GFX_CMD_LINE_TO;
        cmd.point = m_vec2(cmd.rel ? 0 : self->last.x, parse_float(scan));
        cmd.cp = cmd.point;
        break;

    case 'c':
        cmd.type = GFX_CMD_CUBIC_TO;
        cmd.cp1 = parse_coordinate(scan);
        parse_whitespace_or_comma(scan);
        cmd.cp2 = parse_coordinate(scan);
        parse_whitespace_or_comma(scan);
        cmd.point = parse_coordinate(scan);
        break;

    case 's':
        cmd.type = GFX_CMD_CUBIC_TO;
        cmd.cp1 = eval_reflected_cp(self);
        cmd.cp2 = parse_coordinate(scan);
        parse_whitespace_or_comma(scan);
        cmd.point = parse_coordinate(scan);
        break;

    case 'q':
        cmd.type = GFX_CMD_QUADRATIC_TO;
        cmd.cp = parse_coordinate(scan);
        parse_whitespace_or_comma(scan);
        cmd.point = parse_coordinate(scan);
        break;

    case 't':
        cmd.type = GFX_CMD_QUADRATIC_TO;
        cmd.cp = eval_reflected_cp(self);
        cmd.point = parse_coordinate(scan);
        break;

    case 'a':
        cmd.type = GFX_CMD_ARC_TO;

        cmd.rx = parse_float(scan);
        parse_whitespace_or_comma(scan);
        cmd.ry = parse_float(scan);
        parse_whitespace_or_comma(scan);
        cmd.angle = parse_float(scan);
        parse_whitespace_or_comma(scan);
        cmd.flags = parse_arcflags(scan);
        parse_whitespace_or_comma(scan);
        cmd.point = parse_coordinate(scan);
        cmd.cp = cmd.point;
        break;

    default:
        break;
    }

    eval_cmd(self, cmd);
}

void gfx_path_parse(GfxPathParser *self, Scan *scan)
{
    if (scan_skip_word(scan, str$("none")))
    {
        return;
    }

    while (!scan_ended(scan) && scan_curr_is_any(scan, str$(GFX_PATH_CMDS)))
    {
        char cmd = scan_next(scan);

        parse_whitespace(scan);

        do
        {
            parse_cmd(self, scan, cmd);
            parse_whitespace_or_comma(scan);

            // If a moveto is followed by multiple pairs of coordinates,
            // the subsequent pairs are treated as implicit lineto commands.
            if (cmd == 'm')
                cmd = 'l';
            if (cmd == 'M')
                cmd = 'L';
        } while (!scan_ended(scan) && !scan_curr_is_any(scan, str$(GFX_PATH_CMDS)));
    }
}

/* --- Flattener ------------------------------------------------------------ */

static void flatten_line(GfxPathFlattener *self, MVec2 point)
{
    MEdge edge = m_edge_vec2(self->last, point);
    if (m_edge_len(edge) > 0.01)
    {
        self->append(self->ctx, edge);
        self->last = point;
    }
}

static void flatten_cubic_recusive(GfxPathFlattener *self, MVec2 a, MVec2 b, MVec2 c, MVec2 d, int depth)
{
    if (depth > GFX_FLATTEN_MAX_DEPTH)
    {
        return;
    }

    MVec2 delta1 = m_vec2_sub(d, a);
    float delta2 = fabsf((b.x - d.x) * delta1.y - (b.y - d.y) * delta1.x);
    float delta3 = fabsf((c.x - d.x) * delta1.y - (c.y - d.y) * delta1.x);

    if ((delta2 + delta3) * (delta2 + delta3) < GFX_FLATTEN_TOLERANCE * (delta1.x * delta1.x + delta1.y * delta1.y))
    {
        flatten_line(self, d);
        return;
    }

    MVec2 ab = m_vec2_div_v(m_vec2_add(a, b), 2);
    MVec2 bc = m_vec2_div_v(m_vec2_add(b, c), 2);
    MVec2 cd = m_vec2_div_v(m_vec2_add(c, d), 2);
    MVec2 abc = m_vec2_div_v(m_vec2_add(ab, bc), 2);
    MVec2 bcd = m_vec2_div_v(m_vec2_add(bc, cd), 2);
    MVec2 abcd = m_vec2_div_v(m_vec2_add(abc, bcd), 2);

    flatten_cubic_recusive(self, a, ab, abc, abcd, depth + 1);
    flatten_cubic_recusive(self, abcd, bcd, cd, d, depth + 1);
}

static void flatten_cubic(GfxPathFlattener *self, MVec2 cp1, MVec2 cp2, MVec2 point)
{
    flatten_cubic_recusive(self, self->last, cp1, cp2, point, 0);
}

static void flatten_quadratic(GfxPathFlattener *self, MVec2 cp, MVec2 point)
{
    MVec2 previous = self->last;

    MVec2 cp1 = m_vec2_add(previous, m_vec2_mul_v(m_vec2_sub(cp, previous), 2.0f / 3.0f));
    MVec2 cp2 = m_vec2_add(point, m_vec2_mul_v(m_vec2_sub(cp, point), (2.0f / 3.0f)));

    flatten_cubic(self, cp1, cp2, point);
}

static void flatten_arc(GfxPathFlattener *self, float rx, float ry, float angle, int flags, MVec2 point)
{
    // Ported from canvg (https://code.google.com/p/canvg/)
    float x1 = self->last.x; // start point
    float y1 = self->last.y;
    float x2 = point.x;
    float y2 = point.y;

    float dx = x1 - x2;
    float dy = y1 - y2;
    float d = sqrtf(dx * dx + dy * dy);

    if (d < 1e-6f || rx < 1e-6f || ry < 1e-6f)
    {
        // The arc degenerates to a line
        flatten_line(self, point);
        return;
    }

    float rotx = angle / 180.0f * M_PI; // x rotation angle
    float sinrx = sinf(rotx);
    float cosrx = cosf(rotx);

    // Convert to center point parameterization.
    // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

    // 1) Compute x1', y1'
    float x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
    float y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;

    d = m_pow2f(x1p) / m_pow2f(rx) + m_pow2f(y1p) / m_pow2f(ry);

    if (d > 1)
    {
        d = sqrtf(d);
        rx *= d;
        ry *= d;
    }

    // 2) Compute cx', cy'
    float sa = m_pow2f(rx) * m_pow2f(ry) - m_pow2f(rx) * m_pow2f(y1p) - m_pow2f(ry) * m_pow2f(x1p);
    float sb = m_pow2f(rx) * m_pow2f(y1p) + m_pow2f(ry) * m_pow2f(x1p);

    if (sa < 0.0f)
    {
        sa = 0.0f;
    }

    float s = 0.0f;

    if (sb > 0.0f)
    {
        s = sqrtf(sa / sb);
    }

    bool fa = !!(flags & GFX_PATH_LARGE);
    bool fs = !!(flags & GFX_PATH_SWEEP);

    if (fa == fs)
    {
        s = -s;
    }

    float cxp = s * rx * y1p / ry;
    float cyp = s * -ry * x1p / rx;

    // 3) Compute cx,cy from cx',cy'
    float cx = cosrx * cxp - sinrx * cyp + (x1 + x2) / 2.0f;
    float cy = sinrx * cxp + cosrx * cyp + (y1 + y2) / 2.0f;

    // 4) Calculate theta1, and delta theta.
    MVec2 u = m_vec2((x1p - cxp) / rx, (y1p - cyp) / ry);
    MVec2 v = m_vec2((-x1p - cxp) / rx, (-y1p - cyp) / ry);

    float a1 = m_vec2_angle_with(m_vec2(1, 0), u); // Initial angle
    float da = m_vec2_angle_with(u, v);

    if (!fs && da > 0)
    {
        da -= 2 * M_PI;
    }
    else if (fs && da < 0)
    {
        da += 2 * M_PI;
    }

    // Approximate the arc using cubic spline segments.
    MTrans2 t = m_trans2(
        cosrx,
        sinrx,
        -sinrx,
        cosrx,
        cx,
        cy);

    // Split arc into max 90 degree segments.
    // The loop assumes an Iter per end point (including start and end), this +1.
    int ndivs = (int)(fabsf(da) / (M_PI * 0.5f) + 1.0f);
    float hda = (da / (float)ndivs) / 2.0f;
    float kappa = fabsf(4.0f / 3.0f * (1.0f - cosf(hda)) / sinf(hda));

    if (da < 0.0f)
    {
        kappa = -kappa;
    }

    MVec2 current = {};
    MVec2 ptan = {};

    for (int i = 0; i <= ndivs; i++)
    {
        float a = a1 + da * (i / (float)ndivs);

        dx = cosf(a);
        dy = sinf(a);

        MVec2 p = m_trans2_apply_point(t, m_vec2(dx * rx, dy * ry));
        MVec2 tan = m_trans2_apply_vector(t, m_vec2(-dy * rx * kappa, dx * ry * kappa));

        if (i > 0)
        {
            flatten_cubic(self, m_vec2_add(current, ptan), m_vec2_sub(p, tan), p);
        }

        current = p;
        ptan = tan;
    }

    flatten_line(self, point);
}

void gfx_flattener_begin(GfxPathFlattener *self)
{
    self->start = (MVec2){};
    self->last = (MVec2){};
}

void gfx_flattener_flatten(GfxPathFlattener *self, GfxPathCmd cmd)
{
    switch (cmd.type)
    {
    default:
    case GFX_CMD_NOP:
        break;

    case GFX_CMD_MOVE_TO:
        self->start = cmd.point;
        self->last = self->start;
        break;

    case GFX_CMD_CLOSE_PATH:
        flatten_line(self, self->start);
        break;

    case GFX_CMD_LINE_TO:
        flatten_line(self, cmd.point);
        break;

    case GFX_CMD_CUBIC_TO:
        flatten_cubic(self, cmd.cp1, cmd.cp2, cmd.point);
        break;

    case GFX_CMD_QUADRATIC_TO:
        flatten_quadratic(self, cmd.cp, cmd.point);
        break;

    case GFX_CMD_ARC_TO:
        flatten_arc(self, cmd.rx, cmd.ry, cmd.angle, cmd.flags, cmd.point);
        break;
    }
}
