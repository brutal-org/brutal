#include <brutal/base.h>
#include <brutal/log.h>
#include <handover/handover.h>

int br_entry(MAYBE_UNUSED Handover *handover)
{
    log("Hello, world!");

    while (1)
    {
    }

    return 0;
}
