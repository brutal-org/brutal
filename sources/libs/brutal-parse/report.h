
#pragma once

#include <brutal-ds>
#include <brutal-io>

#include "ref.h"
#include "scan.h"
typedef enum
{
    PARSE_REPORT_NONE,
    PARSE_WARN,
    PARSE_ERR,
} ParseReportLevel;

typedef struct
{
    SrcRef ref;
    Str msg;
} ParseReportInfo;

typedef struct
{
    ParseReportLevel level;
    ParseReportInfo info;
    Str report_user_id;
    Vec(ParseReportInfo) comments;
} ParseReport;

typedef struct
{
    Alloc *alloc;
    Vec(ParseReport) reports;
} ParseReports;

void parse_reports_init(ParseReports *self, Alloc *alloc);

void parse_reports_deinit(ParseReports *self);

int parse_report_impl(ParseReports *self, Str id, ParseReportLevel level, SrcRef ref, Str msg);

void parse_report_comment_impl(ParseReports *self, int report_id, SrcRef ref, Str msg);

void parse_report_dump(ParseReport report, Emit *out, Scan const *source, Str filename);
int parse_reports_dump(ParseReports *self, Scan const *scan, Emit *out, Str buf_id);

#define parse_report$(SELF, LEVEL, ID, REF, FMT, ...)                              \
    ({                                                                             \
        Str _report_str = fmt_str((SELF)->alloc, str$(FMT), any_va$(__VA_ARGS__)); \
        parse_report_impl((SELF), (ID), (LEVEL), (REF), _report_str);              \
    })

#define parse_comment$(SELF, REPORT, REF, FMT, ...)                                \
    ({                                                                             \
        Str _report_str = fmt_str((SELF)->alloc, str$(FMT), any_va$(__VA_ARGS__)); \
        parse_report_comment_impl((SELF), (REPORT), (REF), _report_str);           \
    })
