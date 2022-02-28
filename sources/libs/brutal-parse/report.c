#include <brutal/debug.h>
#include <brutal/parse/report.h>

void parse_reports_init(ParseReports *self, Alloc *alloc)
{
    vec_init(&self->reports, alloc);
    self->alloc = alloc;
}

void parse_reports_deinit(ParseReports *self)
{
    vec_deinit(&self->reports);
}

int parse_report_impl(ParseReports *self, Str id, ParseReportLevel level, SrcRef ref, Str msg)
{
    ParseReport rep = {
        .level = level,
        .report_user_id = str_dup(id, self->alloc),
        .info = (ParseReportInfo){
            .msg = msg,
            .ref = ref,
        },
        .comments = {},
    };

    vec_init(&rep.comments, self->alloc);
    vec_push(&self->reports, rep);
    return self->reports.len - 1;
}

void parse_report_comment_impl(ParseReports *self, int report_id, SrcRef ref, Str msg)
{
    assert_lower_than(report_id, self->reports.len);
    ParseReport *report = &self->reports.data[report_id];

    vec_push(&report->comments,
             ((ParseReportInfo){
                 .ref = ref,
                 .msg = msg,
             }));
}

static void parse_report_line(Emit *out, Str src, SrcFilePosInfo info, bool only_first_line)
{
    Str line = str_sub(src, info.char_line_start, info.char_line_end);

    emit_fmt(out, "{3d fg-blue} {fg-blue} ", info.line_number_start, "|");

    if (info.is_multiline && !only_first_line)
    {
        emit_fmt(out, "{fg-red} ", "/");
    }

    int line_number = info.line_number_start;
    for (int i = 0; i < info.char_line_len; i++)
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
            char lined = (char)line.buf[i];
            if (lined == '\t')
            {
                lined = ' ';
            }
            emit_fmt(out, "{}", lined);
        }
    }
}

void parse_report_dump(ParseReport report, Emit *out, Scan const *source, Str filename)
{
    SrcRef src_ref = report.info.ref;
    int begin = src_ref.begin;
    int end = src_ref.end;

    if (report.level == PARSE_WARN)
    {
        emit_fmt(out, "\n! warn: {fg-red}\n", report.info.msg);
    }
    else if (report.level == PARSE_ERR)
    {
        emit_fmt(out, "\n!! {fg-red bold}({fg-red}): {bold}\n", "error", report.report_user_id, report.info.msg);
    }

    Str src = str_n$(source->size, (char *)source->buf);
    SrcFilePosInfo info;
    scan_srcref_info(src_ref, &info, src);
    // number of line from begin
    emit_fmt(out, "-> {fg-blue}:{}:{} \n", filename, info.line_number_start, begin - info.char_line_start);

    parse_report_line(out, src, info, false);

    if (!info.is_multiline)
    {
        emit_fmt(out, "\n    :");

        for (int i = info.char_line_start; i < info.char_line_end; i++)
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

        // if the comment is in the same line as the error, we don't need to print a separate line
        bool is_comment_inline =
            v->ref.begin >= info.char_line_start &&
            v->ref.end <= (info.char_line_end);

        SrcFilePosInfo subinfo = info;

        // if the comment is somewhere else in the code, we need to print the line
        if (info.is_multiline || !is_comment_inline)
        {
            emit_fmt(out, "\n");
            scan_srcref_info(v->ref, &subinfo, src);
            parse_report_line(out, src, subinfo, true);
        }

        emit_fmt(out, "\n    :");

        for (int i = subinfo.char_line_start; i < m_min(subinfo.char_line_end, v->ref.end + 2); i++)
        {
            if (i > v->ref.begin && i <= v->ref.end)
            {
                emit_fmt(out, "{fg-blue}", "-");
            }
            else
            {
                emit_fmt(out, " ");
            }
        }

        emit_fmt(out, " {fg-blue}", v->msg);
    }

    emit_fmt(out, "\n");
}

int parse_reports_dump(ParseReports *self, Scan const *scan, Emit *out, Str buf_id)
{
    int has_error = 0;
    vec_foreach(report, &self->reports)
    {
        if (report->level == PARSE_ERR)
        {
            has_error += 1;
        }
        parse_report_dump(*report, out, scan, buf_id);
    }
    return has_error;
}
