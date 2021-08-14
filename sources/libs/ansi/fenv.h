#pragma once

/* --- 7.6 - Floating-point environment ------------------------------------- */

typedef struct
{
    int _dummy;
} fenv_t;

#define FE_DIVBYZERO (1 << 0)
#define FE_INEXACT (1 << 1)
#define FE_INVALID (1 << 2)
#define FE_OVERFLOW (1 << 3)
#define FE_UNDERFLOW (1 << 4)

#define FE_ALL_EXCEPT 0xff

typedef unsigned char fexcept_t;

#define FE_DOWNWARD (1)
#define FE_TONEAREST (2)
#define FE_TOWARDZERO (3)
#define FE_UPWARD (4)

int feclearexcept(int);

int fegetenv(fenv_t *);

int fegetexceptflag(fexcept_t *, int);

int fegetround(void);

int feholdexcept(fenv_t *);

int feraiseexcept(int);

int fesetenv(fenv_t const *);

int fesetexceptflag(fexcept_t const *, int);

int fesetround(int);

int fetestexcept(int);

int feupdateenv(fenv_t const *);
