#pragma once

#include <stdc-base/prelude.h>

STDC_BEGIN_HEADER

extern int *_errno(void);

/* --- 7.5 Errors ----------------------------------------------------------- */

#define errno (*_errno())

/* Mathematics argument out of domain of function. */
#define EDOM 18

/* Illegal byte sequence. */
#define EILSEQ 25

/* Result too large. */
#define ERANGE 71

STDC_END_HEADER

#if __has_include_next(<errno.h>)
#    include_next <errno.h>
#endif
