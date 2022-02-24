#pragma once

#include <cc/sema/scope.h>

typedef enum
{
    CSEMA_NONE,
    CSEMA_WARN,
    CSEMA_ERR,
} CSemaReportLevel;

typedef struct
{
    CSemaReportLevel level;
    CRef ref;
    Str msg;
} CSemaReport;

typedef struct
{
    Vec(CScope) scopes;
    Vec(CSemaReport) reports;
    Alloc *alloc;
} CSema;

void csema_init(CSema *self, Alloc *alloc);

void csema_deinit(CSema *self);

void csema_scope_enter(CSema *self);

void csema_scope_enter_func(CSema *self, CType func_type);

void csema_scope_leave(CSema *self);

void csema_scope_add(CSema *self, CDecl decl);

CDecl csema_lookup(CSema *self, Str name);

CType csema_scope_return_type(CSema *self);

void csema_report_impl(CSema *self, CSemaReportLevel level, CRef ref, Str msg);

#define csema_report$(SELF, LEVEL, REF, FMT, ...)                                          \
    ({                                                                                     \
        Str _report_str = str_fmt_impl((SELF)->alloc, str$(FMT), PRINT_ARGS(__VA_ARGS__)); \
        csema_report_impl(SELF, LEVEL, (REF).ref, _report_str);                                  \
    })
