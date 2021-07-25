#include <brutal/base.h>
#include <brutal/log.h>
#include <handover/handover.h>

int br_entry(MAYBE_UNUSED Handover *handover)
{
    log("Handover at {#p}", (void *)handover);

    handover_dump(handover);

    while (1)
    {
    }

    return 0;
}
