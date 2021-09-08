#include <brutal/log.h>
#include <syscalls/helpers.h>
#include <syscalls/syscalls.h>

BrResult brh_close(BrHandle handle)
{
    return br_close(&(BrCloseArgs){
        .handle = handle,
    });
}

static BrGlobalInfo *global_infos = nullptr;

BrGlobalInfo *brh_global_info(void)
{
    if (global_infos == nullptr)
    {
        BrMapArgs g_map = {
            .space = BR_SPACE_SELF,
            .mem_obj = BR_MEM_OBJ_GINFO,
        };

        assert_br_success(br_map(&g_map));
        global_infos = (BrGlobalInfo *)g_map.vaddr;
    }

    return global_infos;
}

static BrLocalInfo *local_infos = nullptr;

BrLocalInfo *brh_local_info(void)
{
    if (global_infos == nullptr)
    {
        BrMapArgs l_map = {
            .space = BR_SPACE_SELF,
            .mem_obj = BR_MEM_OBJ_LINFO,
        };

        assert_br_success(br_map(&l_map));
        local_infos = (BrLocalInfo *)l_map.vaddr;
    }

    return local_infos;
}
