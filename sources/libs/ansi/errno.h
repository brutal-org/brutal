#pragma once

extern int *_get_errno(void);

/* --- 7.5 Errors ----------------------------------------------------------- */

#define EDOM ((int)33)

#define ERANGE ((int)34)

#define EILSEQ ((int)84)

#define errno (*_get_errno())

#if __has_include_next(<errno.h>)
#    include_next <errno.h>
#endif
