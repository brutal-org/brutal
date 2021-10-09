#include <brutal/alloc.h>
#include <cc/builder/helper.h>

CStmt cc_var_decl_str(Str type, Str name, CExpr set, Alloc *alloc)
{

    return cstmt_decl(cdecl_var(str$(name),
                                ctype_name(str$(type), alloc),
                                set, alloc),
                      alloc);
}

CExpr cc_func_call(Str call_func, Alloc *alloc)
{
    return cexpr_call(alloc, cexpr_identifier(call_func, alloc));
}
CExpr cc_access_str(Str from, Str targ, Alloc *alloc)
{
    return cexpr_access(cexpr_identifier(from, alloc), cexpr_identifier(targ, alloc), alloc);
}

CExpr cc_ptr_access_str(Str from, Str targ, Alloc *alloc)
{
    return cexpr_ptr_access(cexpr_identifier(from, alloc), cexpr_identifier(targ, alloc), alloc);
}

CExpr cc_index_constant(CExpr v, int idx, Alloc *alloc)
{
    return cexpr_index(v, cexpr_constant(cval_cast(idx)), alloc);
}
void cc_push_initializer_member(CExpr *targ, Str designator, Str value, Alloc *alloc)
{
    CExpr get = cexpr_identifier(designator, alloc);

    CExpr set = cexpr_identifier(value, alloc);
    cexpr_initializer_push_element(targ, get, set, alloc);
}
