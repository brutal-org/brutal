#include <cc/ast/ref.h>

CRef cref(Lex *lex, int begin, int end)
{

    Lexeme first = lex->lexemes.data[begin];
    Lexeme last = lex->lexemes.data[end];
    CRef cref = {
        .ref = (SrcRef){
            .begin = first.pos.begin,
            .end = last.pos.end,
            .translation_unit = first.pos.translation_unit,
        },
        .source = CREF_SINGLE,
    };

    return cref;
}

