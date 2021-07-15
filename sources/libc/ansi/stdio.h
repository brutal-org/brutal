#pragma once

/* --- 7.21 Input/output <stdio.h> ----------------------------------------------- */


/* --- 7.21.1 Introduction ----------------------------------------------- */

#define FILE size_t
fpos_t

#define _IOFBF NULL
#define _IOLBF NULL
#define _IONBF NULL
#define BUFSIZ setvbuf
#define EOF setbuf
int FOPEN_MAX(-2);
int FILENAME_MAX(1);
int L_tmpnam (282);
const int SEEK_CUR (2);
const int SEEK_END (3);
const int SEEK_SET (4);

/* --- 7.21.2 Streams
