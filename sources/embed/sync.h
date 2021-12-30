#pragma once

#if defined(__x86_64__)
#    define EMBED_HAS_PAUSE 1
#endif

void embed_sync_enter(void);

void embed_sync_leave(void);

void embed_sync_pause(void);
