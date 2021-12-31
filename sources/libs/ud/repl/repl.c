#include <ud/repl.h>

#ifdef USE_READLINE
// clang-format off
#    include <stdio.h>
#    include <readline/readline.h>
# 	 include <readline/history.h>
#	 include <stdlib.h>
// clang-format on
#endif

Str ud_read_line(Str prompt)
{
#ifdef USE_READLINE
    char *str = readline(prompt.buf);

    add_history(str);

    return str$(str);
#else
    log$("BRUTAL does not support readline");
    return prompt;
#endif
}

void ud_do_repl_cleanup(MAYBE_UNUSED Str expr)
{
#ifdef USE_READLINE
    free((void *)expr.buf);
#endif
}