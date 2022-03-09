#pragma once

#include <brutal/parse/report.h>
#include <cc/sema/scope.h>

typedef struct
{
    Vec(CScope) scopes;
    ParseReports reports;
    Alloc *alloc;

    Str current_pass;
} CSema;

void csema_init(CSema *self, Alloc *alloc);

void csema_deinit(CSema *self);

void csema_scope_enter(CSema *self);

void csema_scope_enter_func(CSema *self, CType func_type);

void csema_scope_leave(CSema *self);

void csema_scope_reset(CSema *self);

void csema_scope_add(CSema *self, CDecl decl);

bool csema_lookup(CSema *self, Str name, CDecl *decl);

bool csema_lookup_current_scope(CSema *self, Str name, CDecl *decl);

CType csema_scope_return_type(CSema *self);

#define csema_report$(SELF, LEVEL, REF, FMT, ...)                                                 \
    ({                                                                                            \
        Str _report_str = str_fmt_impl((SELF)->alloc, str$(FMT), PRINT_ARGS(__VA_ARGS__));        \
        parse_report_impl(&(SELF)->reports, (SELF)->current_pass, LEVEL, (REF).ref, _report_str); \
    })

#define csema_comment$(SELF, REPORT, REF, FMT, ...)                                        \
    ({                                                                                     \
        Str _report_str = str_fmt_impl((SELF)->alloc, str$(FMT), PRINT_ARGS(__VA_ARGS__)); \
        parse_report_comment_impl(&(SELF)->reports, REPORT, (REF).ref, _report_str);       \
    })
