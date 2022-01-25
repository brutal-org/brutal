#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/builder.h>
#include <idl/cgen.h>

void idl_cgen_includes(CUnit *unit, IdlModule const module, Alloc *alloc)
{
    vec_foreach_v(include, &module.includes)
    {
        cunit_include(unit, true, include);
    }

    vec_foreach_v(import, &module.imports)
    {
        cunit_include(unit, true, str_fmt(alloc, "protos/{}.h", import.name));
    }
}
