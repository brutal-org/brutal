#pragma once

#if __SIZEOF_INT128__ == 16 && defined(__x86_64__)
typedef __int128_t Int128;
typedef __uint128_t UInt128;
#endif
