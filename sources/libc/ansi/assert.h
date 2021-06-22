#pragma once

/* ---define assert in terms of NDEBUG ----------------------------------------------- */ 

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#define assert(cond) 
#endif



/* --- static assert ----------------------------------------------- */ 


#define static_assert _Static_assert
