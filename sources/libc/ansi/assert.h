#pragma once

/* ---define assert in terms of NDEBUG ----------------------------------------------- */ 

#ifdef NDEBUG
#define asser(ignore) ((void)0)
#else
#define asser(cond) 
#endif



/* --- static assert ----------------------------------------------- */ 


#define static_assert _Static_assert
