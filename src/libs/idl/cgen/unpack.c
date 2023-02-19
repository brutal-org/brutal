#include <brutal-fmt>
#include <cc/builder.h>
#include <idl/cgen.h>

Str idl_cgen_unpack_name(IdlAlias alias, Alloc *alloc)
{
    IdlAttr attr;
    if (idl_attrs_get(alias.attrs, str$("packfn"), &attr))
    {
        return vec_at(&attr.args, 1);
    }

    return fmt_str$(alloc, "ipc_unpack_{case-snake}", alias.name);
}

CExpr idl_cgen_unpack_ref(IdlAlias alias, Alloc *alloc)
{
    return cexpr_cast(cexpr_ident(idl_cgen_unpack_name(alias, alloc)), ctype_ident_ptr(str$("IpcUnpackFn"), alloc), alloc);
}

CType idl_cgen_unpack_type(IdlAlias alias, Alloc *alloc)
{
    CType ctype = ctype_func(ctype_void(), alloc);
    ctype_member(&ctype, str$("self"), ctype_ident_ptr(str$("IpcUnpack"), alloc));
    ctype_member(&ctype, str$("data"), ctype_ptr(idl_cgen_alias_type(alias), alloc));
    return ctype;
}

void idl_cgen_unpack_body(CStmt *block, IdlType type, CExpr path, Alloc *alloc)
{
    switch (type.type)
    {
    case IDL_TYPE_NIL:
        break;

    case IDL_TYPE_ENUM:
    case IDL_TYPE_CTYPE:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(str$("ipc_unpack_pod")));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);

        cstmt_block_add(block, cstmt_expr(expr));
        break;
    }

    case IDL_TYPE_PRIMITIVE:
    {
        CExpr expr = cexpr_call(alloc, cexpr_ident(idl_cgen_unpack_name(*type.primitive_.alias, alloc)));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);

        cstmt_block_add(block, cstmt_expr(expr));
        break;
    }

    case IDL_TYPE_STRUCT:
    {
        vec_foreach_v(member, &type.struct_.members)
        {
            idl_cgen_unpack_body(block, member.type, cexpr_ref(cexpr_ptr_access(path, cexpr_ident(member.name), alloc), alloc), alloc);
        }
        break;
    }

    case IDL_TYPE_VEC:
    {
        IdlType subtype = *type.vec_.subtype;
        assert_truth(subtype.type == IDL_TYPE_PRIMITIVE);

        CExpr expr = cexpr_call(alloc, cexpr_ident(str$("ipc_unpack_slice")));
        cexpr_member(&expr, cexpr_ident(str$("self")));
        cexpr_member(&expr, path);
        cexpr_member(&expr, cexpr_ident(idl_cgen_unpack_name(*subtype.primitive_.alias, alloc)));
        cstmt_block_add(block, cstmt_expr(expr));
        break;
    }

    default:
        panic$("Unknow type type {}", type.type);
    }
}

CDecl idl_cgen_unpack_decl(IdlAlias alias, Alloc *alloc)
{
    Str name = idl_cgen_unpack_name(alias, alloc);
    CType type = idl_cgen_unpack_type(alias, alloc);

    return cdecl_func(name, type, cstmt_empty());
}

CDecl idl_cgen_unpack_func(IdlAlias alias, Alloc *alloc)
{
    Str name = idl_cgen_unpack_name(alias, alloc);
    CType type = idl_cgen_unpack_type(alias, alloc);
    CStmt body = cstmt_block(alloc);
    idl_cgen_unpack_body(&body, alias.type, cexpr_ident(str$("data")), alloc);

    return cdecl_func(name, type, body);
}
