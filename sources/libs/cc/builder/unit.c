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

void cunit_decl(CUnit *self, CDecl decl)
{
    cunit_member(
        self,
        (CUnitEntry){
            .type = CUNIT_DECLARATION,
            ._decl = decl,
        });
}

void cunit_pragma(CUnit *self, Str value, Alloc *alloc)
{
    CPragma pragma = {
        .text = str_dup(value, alloc),
    };

    cunit_member(
        self,
        (CUnitEntry){
            .type = CUNIT_PRAGMA,
            ._pragma = pragma,
        });
}

void cunit_pragma_once(CUnit *self, Alloc *alloc)
{
    cunit_pragma(self, str$("once"), alloc);
}

void cunit_include(CUnit *self, bool system, Str path, Alloc *alloc)
{
    cunit_member(
        self,
        (CUnitEntry){
            .type = CUNIT_INCLUDE,
            ._include = (CInclude){
                .is_system = system,
                .path = str_dup(path, alloc),
            },
        });
}

void cunit_define(CUnit *self, Str name, CExpr expression, Alloc *alloc)
{
    CUnitEntry def = {
        .type = CUNIT_DEFINE,
        ._define = (CDefine){
            .expression = expression,
            .name = str_dup(name, alloc),
        },
    };

    vec_init(&def._define.args, alloc);

    cunit_member(self, def);
}
