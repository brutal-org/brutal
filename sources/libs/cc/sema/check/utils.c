#include <cc/sema/check/utils.h>

Str csema_ctype_to_str(CExpr expr, Alloc *alloc)
{
    Emit emit;
    Buf buf = {};
    buf_init(&buf, 18, alloc);
    emit_init(&emit, buf_writer(&buf));

    cc_trans_type(&emit, expr.sema_type);
    emit_deinit(&emit);

    return str_n$(buf.used, (char *)buf.data);
}
