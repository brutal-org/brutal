
// here you register every sema check
// note: the order is important

#include <cc/sema.h>
#include <cc/sema/check/array_type.h>
#include <cc/sema/check/call.h>
#include <cc/sema/check/decl.h>
#include <cc/sema/check/deref_type.h>
#include <cc/sema/checks.h>

SemaCheck check_list[] = {
    SEMA_DECL_REF_VALIDATION,
    SEMA_DECL_DEF_VALIDATION,
    SEMA_ARRAY_TYPE_VALIDATION,
    SEMA_CALL_VALIDATION,
    SEMA_DEREF_VALIDATION,

};

void do_sema_checks(CUnit *unit, CSema *sema)
{
    for (size_t i = 0; i < sizeof(check_list) / sizeof(check_list[0]); i++)
    {
        csema_scope_reset(sema);
        sema->current_pass = str$(check_list[i].name);
        do_sema_check(&check_list[i], unit, sema);
    }
}
