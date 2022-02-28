#include <brutal/parse/ref.h>

void scan_srcref_info(SrcRef ref, SrcFilePosInfo *info, Str const src)
{
    int begin = ref.begin;
    int end = ref.end;
    int line_len;

    int line_number_start = str_count_chr(str_sub(src, 0, begin), '\n') + 1;
    int line_number_end = str_count_chr(str_sub(src, 0, end), '\n') + 1;
    int line_start = str_last_chr(str_sub(src, 0, begin), '\n') + 1;
    int line_end = str_first_chr(str_sub(src, end, src.len), '\n') + end;
    bool is_multiline = line_number_start != line_number_end;

    if (!is_multiline)
    {
        line_len = str_first_chr(str_sub(src, line_start, src.len), '\n');
    }
    else
    {
        line_len = str_last_chr(str_sub(src, line_start, line_end + 1), '\n');
    }

    if (line_len == -1)
    {
        line_len = src.len;
    }

    *info = (SrcFilePosInfo){
        .is_multiline = is_multiline,
        .char_line_len = line_len,
        .char_line_end = line_end,
        .char_line_start = line_start,
        .line_number_end = line_number_end,
        .line_number_start = line_number_start,
    };
}
