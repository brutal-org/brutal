#include <embed/sync.h>

void embed_sync_pause(void)
{
    asm volatile("pause");
}
