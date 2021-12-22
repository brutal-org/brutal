#pragma once

extern int *_errno(void);

/* --- 7.5 Errors ----------------------------------------------------------- */

#define errno (*_errno())

/* Mathematics argument out of domain of function. */
#define EDOM 18

/* Illegal byte sequence. */
#define EILSEQ 25

/* Result too large. */
#define ERANGE 71

#if __has_include_next(<errno.h>)
#    include_next <errno.h>
#endif
