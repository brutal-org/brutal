#pragma once

/* --- 7.5 Errors ----------------------------------------------------------- */

#define EDOM (2)

#define EILSEQ (3)

#define ERANGE (4)

extern int errno = (0);

#if __has_include_next(<errno.h>)
#    include_next <errno.h>
#endif
