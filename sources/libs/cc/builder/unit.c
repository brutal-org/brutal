#include <cc/builder/unit.h>

CUnit cunit(Alloc *alloc)
{
    CUnit unit;
    vec_init(&unit.units, alloc);
    return unit;
}

void cunit_member(CUnit *unit, CUnitEntry entry)
{
    vec_push(&unit->units, entry);
}

CUnitEntry cunit_decl(CDecl decl)
{
    return (CUnitEntry){
        .type = CUNIT_DECLARATION,
        ._decl = decl,
    };
}

CUnitEntry cunit_pragma(Str value, Alloc *alloc)
{
    CPragma pragma = {
        .text = str_dup(value, alloc),
    };

    return (CUnitEntry){
        .type = CUNIT_PRAGMA,
        ._pragma = pragma,
    };
}

CUnitEntry cunit_pragma_once(Alloc *alloc)
{
    return cunit_pragma(str$("once"), alloc);
}

CUnitEntry cunit_include(bool qchar, Str path, Alloc *alloc)
{
    return (CUnitEntry){
        .type = CUNIT_INCLUDE,
        ._include = (CInclude){
            .is_q_char = qchar,
            .path = str_dup(path, alloc),
        },
    };
}

CUnitEntry cunit_define(Str name, CExpr expression, Alloc *alloc)
{
    CUnitEntry def = {
        .type = CUNIT_DEFINE,
        ._define = (CDefine){
            .expression = expression,
            .name = str_dup(name, alloc),
        },
    };

    vec_init(&def._define.args, alloc);

    return def;
}
