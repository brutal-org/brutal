#include <cc/builder/unit.h>

bool cunit_contains_type(CUnit *context, Str name)
{
    vec_foreach_v(entry, &context->units)
    {
        if (entry.type == CUNIT_DECLARATION && entry._decl.type == CDECL_TYPE && str_eq(entry._decl.name, name))
        {
            return true;
        }
    }
    return false;
}

bool cunit_contains_decl(CUnit *context, Str name)
{
    vec_foreach_v(entry, &context->units)
    {
        if (entry.type == CUNIT_DECLARATION && entry._decl.type != CDECL_TYPE && str_eq(entry._decl.name, name))
        {
            return true;
        }
    }
    return false;
}

/* --- Builders ------------------------------------------------------------- */

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

void cunit_pragma(CUnit *self, Str value)
{
    CPragma pragma = {
        .text = value,
    };

    cunit_member(
        self,
        (CUnitEntry){
            .type = CUNIT_PRAGMA,
            ._pragma = pragma,
        });
}

void cunit_pragma_once(CUnit *self)
{
    cunit_pragma(self, str$("once"));
}

void cunit_include(CUnit *self, bool system, Str path)
{
    cunit_member(
        self,
        (CUnitEntry){
            .type = CUNIT_INCLUDE,
            ._include = (CInclude){
                .is_system = system,
                .path = path,
            },
        });
}

void cunit_define(CUnit *self, Str name, CExpr expression, Alloc *alloc)
{
    CUnitEntry def = {
        .type = CUNIT_DEFINE,
        ._define = (CDefine){
            .expression = expression,
            .name = name,
        },
    };

    vec_init(&def._define.args, alloc);

    cunit_member(self, def);
}
