#pragma once

#include <brutal/ds/vec.h>
#include <brutal/io/buf.h>
#include <brutal/text/str.h>

typedef int SrcPos;

typedef struct src Src;

typedef struct
{
    Src *src;
    SrcPos start;
    SrcPos end;
} SrcSpan;

typedef enum
{
    SRC_DIAG_ERROR,
    SRC_DIAG_WARNING,
    SRC_DIAG_HELP,
} SrcDiagType;

typedef struct
{
    SrcDiagType type;
    SrcSpan span;
    Str text;
} SrcDiag;

typedef struct
{
    int type;
    SrcSpan span;
    Str str;
} SrcTok;

struct src
{
    Str name;
    Buf text;

    Vec(Str) lines;
    Vec(SrcDiag) diags;
    Vec(SrcTok) toks;
};

void src_init(Src *self, Str name, Str text, Alloc *alloc);

void src_deinit(Src *self);

Str src_line(Src *self, int line);