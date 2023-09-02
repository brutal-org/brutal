#include <cc/trans.h>

static void ctrans_include(Emit *emit, CInclude path)
{
    emit_fmt$(emit, "#include ");

    if (path.is_system)
    {
        emit_fmt$(emit, "<{}>\n", path.path);
    }
    else
    {
        emit_fmt$(emit, "\"{}\"\n", path.path);
    }
}

static void ctrans_pragma(Emit *emit, CPragma pragma)
{
    emit_fmt$(emit, "#pragma ");
    emit_fmt$(emit, pragma.text);
    emit_fmt$(emit, "\n");
}

static void ctrans_define(Emit *emit, CDefine define)
{
    emit_fmt$(emit, "#define ");
    emit_fmt$(emit, define.name);
    if (define.args.len != 0)
    {
        emit_fmt$(emit, "(");

        for (int i = 0; i < define.args.len; i++)
        {
            if (i != 0)
            {
                emit_fmt$(emit, ", {}", define.args.data[i]);
            }
            else
            {
                emit_fmt$(emit, "{}", define.args.data[i]);
            }
        }

        emit_fmt$(emit, ")");
    }
    emit_fmt$(emit, " ");
    ctrans_expr(emit, define.expression);
    emit_fmt$(emit, "\n");
}

void ctrans_unit(Emit *emit, CUnit unit)
{
    CUnitType prev_type = CUNIT_INVALID;

    vec_foreach_v(entry, &unit.units)
    {
        switch (entry.type)
        {
        case CUNIT_INCLUDE:
            if (prev_type != entry.type)
            {
                emit_fmt$(emit, "\n");
            }

            ctrans_include(emit, entry._include);
            break;

        case CUNIT_PRAGMA:
            ctrans_pragma(emit, entry._pragma);
            break;

        case CUNIT_DECLARATION:
            emit_fmt$(emit, "\n");

            ctrans_decl(emit, entry._decl);

            if (entry._decl.type != CDECL_FUNC)
            {
                emit_fmt$(emit, ";");
            }

            emit_fmt$(emit, "\n");
            break;

        case CUNIT_DEFINE:
            if (prev_type != entry.type)
            {
                emit_fmt$(emit, "\n");
            }

            ctrans_define(emit, entry._define);
            break;

        default:
            break;
        }

        prev_type = entry.type;
    }
}
