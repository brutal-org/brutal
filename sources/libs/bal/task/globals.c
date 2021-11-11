#include <bal/abi/syscalls.h>
#include <bal/task/globals.h>
#include <brutal/debug/assert.h>

static BrGlobals *globals = nullptr;
static BrLocals *locals = nullptr;

BrGlobals *bal_globals(void)
{
    if (!globals)
    {
        BrMapArgs g_map = {
            .space = BR_SPACE_SELF,
            .mem_obj = BR_MEM_OBJ_GINFO,
        };

        assert_br_success(br_map(&g_map));
        globals = (BrGlobals *)g_map.vaddr;
    }

    return globals;
}

BrLocals *bal_locals(void)
{
    if (!locals)
    {
        BrMapArgs l_map = {
            .space = BR_SPACE_SELF,
            .mem_obj = BR_MEM_OBJ_LINFO,
        };

        assert_br_success(br_map(&l_map));
        locals = (BrLocals *)l_map.vaddr;
    }

    return locals;
}
