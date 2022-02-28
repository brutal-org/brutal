#include <brutal-debug>
#include <brutal-fmt>
#include <cc/dump.h>
#include <cc/lex/pproc.h>
#include <cc/parse.h>
#include <cc/sema.h>
#include <cc/sema/checks.h>
#include <cc/trans.h>
#include <json/emit.h>

typedef struct
{
    int line_number_start; // the number of the line at the start
    int line_number_end;   // the number of the line at the end
    int line_start;        // the starting character index  aligned by a line
    int line_end;          // the last character index aligned by a line
    int line_len;          // the length of the line(s)
    bool is_multiline;     // if the range is represented by multiple or a single line
} SrcFilePosInfo;

// FIXME: this will be moved in an other file later.
SrcFilePosInfo report_info(SrcRef ref, Str src)
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

    return (SrcFilePosInfo){
        .is_multiline = is_multiline,
        .line_len = line_len,
        .line_end = line_end,
        .line_start = line_start,
        .line_number_end = line_number_end,
        .line_number_start = line_number_start,
    };
}

void report_csema_line(Emit *out, Str src, SrcFilePosInfo info, bool only_first_line)
{
    Str line = str_sub(src, info.line_start, info.line_start + info.line_len);
    emit_fmt(out, "{3d fg-blue} {fg-blue} ", info.line_number_start, "|");

    if (info.is_multiline && !only_first_line)
    {
        emit_fmt(out, "{fg-red} ", "/");
    }

    int line_number = info.line_number_start;
    for (int i = 0; i < info.line_len; i++)
    {
        if (line.buf[i] == '\n')
        {
            if (only_first_line)
            {
                break;
            }
            line_number++;
            emit_fmt(out, "\n{3d fg-blue} {fg-blue} ", line_number, "|");

            if (info.is_multiline)
            {
                emit_fmt(out, "{fg-red} ", "|");
            }
        }
        else
        {
            emit_fmt(out, "{}", line.buf[i]);
        }
    }
}

void report_csema_dump(CSemaReport report, Emit *out, Scan *source, Str filename)
{
    SrcRef src_ref = report.info.cref;
    int begin = src_ref.begin;
    int end = src_ref.end;

    if (report.level == CSEMA_WARN)
    {
        emit_fmt(out, "\n! warn: {red}\n", report.info.msg);
    }
    else if (report.level == CSEMA_ERR)
    {
        emit_fmt(out, "\n!! {fg-red bold}({fg-red}): {bold}\n", "error", report.pass, report.info.msg);
    }

    Str src = str_n$(source->size, (char *)source->buf);
    SrcFilePosInfo info = report_info(src_ref, src);
    // number of line from begin
    emit_fmt(out, "-> {fg-blue}:{}:{} \n", filename, info.line_number_start, begin - info.line_start);

    report_csema_line(out, src, info, false);

    if (!info.is_multiline)
    {
        emit_fmt(out, "\n    : ");

        for (int i = info.line_start; i < info.line_len + info.line_start; i++)
        {
            if (i > begin && i <= end)
            {
                emit_fmt(out, "{fg-red}", "^");
            }
            else
            {
                emit_fmt(out, " ");
            }
        }
        emit_fmt(out, " {fg-red}", report.info.msg);
    }

    vec_foreach(v, &report.comments)
    {

        bool is_comment_inline = v->cref.begin >= info.line_start && v->cref.end <= (info.line_len + info.line_start);

        if (!info.is_multiline && is_comment_inline)
        {
            emit_fmt(out, "\n    : ");

            for (int i = info.line_start; i < m_min(info.line_len + info.line_start, v->cref.end + 2); i++)
            {
                if (i > v->cref.begin && i <= v->cref.end)
                {
                    emit_fmt(out, "{fg-blue}", "-");
                }
                else
                {
                    emit_fmt(out, " ");
                }
            }
        }
        else
        {
            emit_fmt(out, "\n");
            SrcFilePosInfo info2 = report_info(v->cref, src);
            report_csema_line(out, src, info2, true);

            emit_fmt(out, "\n    : ");
            for (int i = info2.line_start; i < m_min(info2.line_len + info2.line_start, v->cref.end + 2); i++)
            {
                if (i > v->cref.begin && i <= v->cref.end)
                {
                    emit_fmt(out, "{fg-blue}", "-");
                }
                else
                {
                    emit_fmt(out, " ");
                }
            }
            emit_fmt(out, " ");
        }
        emit_fmt(out, "{fg-blue}", v->msg);
    }

    emit_fmt(out, "\n");
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("Usages: {} [input]", argv[0]);
        return 0;
    }

    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_view(&source_file, str$(argv[1])), "File not found!");

    Buf source_buf;
    buf_init(&source_buf, 512, base$(&heap));

    io_copy(io_file_reader(&source_file), buf_writer(&source_buf));

    Scan scan;
    scan_init(&scan, buf_str(&source_buf));

    Lex unprocessed = clex(&scan, base$(&heap), 0);
    Lex processed = cproc_file(&unprocessed, str$(argv[1]), base$(&heap));

    if (scan_dump_error(&scan, io_chan_err()))
    {
        return -1;
    }

    CUnit unit = cparse_unit(&processed, base$(&heap));
    CSema sema;
    csema_init(&sema, base$(&heap));

    unit = csema_unit(&sema, unit, base$(&heap));

    do_sema_checks(&unit, &sema);

    Emit emit;
    emit_init(&emit, io_chan_out());

    parse_reports_dump(&sema.reports, &scan, &emit, str$(argv[1]));

    emit_fmt(&emit, "\n");

    emit_fmt$(&emit, "--- BEGIN OG CODE ---\n");
    emit_fmt$(&emit, "{}\n", buf_str(&source_buf));
    emit_fmt$(&emit, "--- END OG CODE ---\n");

    emit_fmt$(&emit, "\n");
    emit_fmt$(&emit, "\n");

    emit_ident_size(&emit, 2);
    emit_fmt$(&emit, "--- BEGIN AST ---\n");
    Json json = cdump_unit(unit, base$(&heap));
    json_emit(json, &emit);
    emit_fmt$(&emit, "--- END AST ---\n");

    emit_fmt$(&emit, "\n");
    emit_fmt$(&emit, "\n");

    emit_ident_size(&emit, 4);

    emit_fmt$(&emit, "--- BEGIN CODE ---\n");
    cc_trans_unit(&emit, unit);
    emit_fmt$(&emit, "--- END CODE ---\n");
    emit_fmt$(&emit, "\n");

    heap_alloc_deinit(&heap);

    return 0;
}
