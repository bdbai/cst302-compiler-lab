%option c++ noyywrap
%option yyclass="Lexer"
%{
#include <cstdlib>
#include <string>
#include <memory>
using namespace std;
#undef YY_DECL
#define YY_DECL yy::parser::symbol_type Lexer::yylex(int drv)
#include "lexer.h"
void yyerror(const string &);
%}

%%
[a-z]           return yy::parser::make_VARIABLE(string(yytext));
0               return yy::parser::make_INTEGER(atoi(yytext));
[1-9][0-9]*     return yy::parser::make_INTEGER(atoi(yytext));
[-()<>=+*/;{}.] return *yytext;
">="            return yy::parser::token::GE;
"<="            return yy::parser::token::LE;
"=="            return yy::parser::token::EQ;
"!="            return yy::parser::token::NE;
"while"         return yy::parser::token::WHILE;
"if"            return yy::parser::token::IF;
"else"          return yy::parser::token::ELSE;
"print"         return yy::parser::token::PRINT;
[ \t\n]+        ;       /* ignore whitespace */
.               yyerror(string("Unknown character"));
%%

int yywrap(void) {
    return 1;
}
