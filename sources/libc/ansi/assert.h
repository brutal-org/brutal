#pragma once

/* --- 7.2 Diagnostics ----------------------------------------------- */ 

NDEBUG

#define assert(ignore) ((void)0)

static_assert

/* --- 7.2.1 Program diagnostics ----------------------------------------------- */ 

void assert(scalar expression);
