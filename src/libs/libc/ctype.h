#pragma once

#include <stdc-base/prelude.h>

/* --- 7.4 Character - handling --------------------------------------------- */

STDC_BEGIN_HEADER

/* --- 7.4.1 Character classification - Copying functions ------------------- */

int isalnum(int c);

int isalpha(int c);

int isblank(int c);

int iscntrl(int c);

int isdigit(int c);

int isgraph(int c);

int islower(int c);

int isprint(int c);

int ispunct(int c);

int isspace(int c);

int isupper(int c);

int isxdigit(int c);

int tolower(int c);

int toupper(int c);

STDC_END_HEADER
