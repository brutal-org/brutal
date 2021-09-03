#pragma once

extern int *_get_errno(void);

/* --- 7.5 Errors ----------------------------------------------------------- */

#define errno (*_get_errno())

/* Mathematics argument out of domain of function. */
#define EDOM 18

/* Illegal byte sequence. */
#define EILSEQ 25

/* Result too large. */
#define ERANGE 71

#if __has_include_next(<errno.h>)
#    include_next <errno.h>
#endif
