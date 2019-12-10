#ifndef LEXER_H
#define LEXER_H
#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#include <FlexLexer.h>
#endif
#include "y.tab.hh"
class Lexer : public yyFlexLexer {
  public:
    static bool isInterpreter;
    static bool isCompiler;
    static bool isGraph;
    yy::parser::symbol_type yylex(int drv);
    int yywrap() override { return 1; }
};
#endif