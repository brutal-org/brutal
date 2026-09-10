#pragma once

#include <brutal-text>
#include <stdint.h>

typedef struct
{
    int translation_unit;
    int begin;
    int end;
} SrcRef;

typedef struct
{
    int line_number_start; // the number of the line at the start
    int line_number_end;   // the number of the line at the end
    int char_line_start;   // the starting character index  aligned by a line
    int char_line_end;     // the last character index aligned by a line
    int char_line_len;     // the length of the line(s)
    bool is_multiline;     // if the range is represented by multiple or a single line
} SrcFilePosInfo;

void scan_srcref_info(SrcRef ref, SrcFilePosInfo *info, Str const src);
