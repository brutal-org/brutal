#include <cc/gen/dump.h>

void cdump_value(Emit *emit, CVal value);

void cdump_type(Emit *emit, CType type)
{
    emit_fmt(emit, "type:");
    emit_ident(emit);

    switch (type.type)
    {
    case CTYPE_INVALID:
        emit_fmt(emit, "<invalid>\n");
        break;

    case CTYPE_TAIL:
        emit_fmt(emit, "<tail>\n");
        break;

    case CTYPE_ERROR:
        emit_fmt(emit, "<error>\n");
        break;

    case CTYPE_VOID:
        emit_fmt(emit, "void\n");
        break;

    case CTYPE_AUTO:
        emit_fmt(emit, "auto\n");
        break;

    case CTYPE_BOOL:
        emit_fmt(emit, "Bool\n");
        break;

    case CTYPE_PTR:
        emit_fmt(emit, "ptr\n");
        cdump_type(emit, *type.ptr_.subtype);
        break;

    case CTYPE_PARENT:
        emit_fmt(emit, "parent\n");
        cdump_type(emit, *type.parent_.subtype);
        break;

    case CTYPE_ARRAY:
        emit_fmt(emit, "array[{}]\n", type.array_.size);
        cdump_type(emit, *type.array_.subtype);
        break;

    case CTYPE_SIGNED:
        emit_fmt(emit, "signed\n");
        break;

    case CTYPE_UNSIGNED:
        emit_fmt(emit, "unsigned\n");
        break;

    case CTYPE_FLOAT:
        emit_fmt(emit, "float\n");
        break;

    case CTYPE_STRUCT:
        emit_fmt(emit, "struct\n");
        break;

    case CTYPE_UNION:
        emit_fmt(emit, "union\n");
        break;

    case CTYPE_ENUM:
        emit_fmt(emit, "enum\n");
        break;

    case CTYPE_FUNC:
        emit_fmt(emit, "func\n");
        break;

    case CTYPE_NAME:
        emit_fmt(emit, "name\n");
        break;

    default:
        assert_unreachable();
    }

    emit_deident(emit);
}

void cdump_expr(Emit *emit, CExpr expr);

void cdump_stmt(Emit *emit, CStmt stmt);

void cdump_decl(Emit *emit, CDecl decl)
{
    emit_fmt(emit, "decl:");
    emit_ident(emit);
    if (decl.type == CDECL_TYPE)
    {
        emit_fmt(emit, "typedef");
        emit_fmt(emit, " {}\n", decl.name);
        cdump_type(emit, decl.type_.type);
    }
    emit_deident(emit);
}

void cdump_unit(Emit *emit, CUnit unit)
{
    emit_fmt(emit, "cunit\n");
    emit_ident(emit);

    vec_foreach(entry, &unit.units)
    {
        if (entry.type == CUNIT_DECLARATION)
        {
            cdump_decl(emit, entry._decl);
        }
    }

    emit_deident(emit);
}
