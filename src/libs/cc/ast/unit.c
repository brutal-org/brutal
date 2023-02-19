#include <cc/ast/unit.h>

bool cunit_contains_type(CUnit *context, Str name)
{
    vec_foreach_v(entry, &context->units)
    {
        if (entry.type == CUNIT_DECLARATION && str_eq(entry._decl.name, name))
        {
            return true;
        }
    }
    return false;
}
