%option c++ noyywrap stack
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

%s BLOCK_END
%%
"while"                 return yy::parser::token::WHILE;
"if"                    return yy::parser::token::IF;
"else"                  return yy::parser::token::ELSE;
"print"                 return yy::parser::token::PRINT;
[_a-zA-Z][_a-zA-Z0-9]*  return yy::parser::make_VARIABLE(string(yytext));
0                       return yy::parser::make_INTEGER(atoi(yytext));
[1-9][0-9]*             return yy::parser::make_INTEGER(atoi(yytext));
-?(0|[1-9][[:digit:]]*)\.([[:digit:]]*)?([eE][-+]?\d*)? {
                        return yy::parser::make_DECIMAL(stod(yytext));
}
"/*"                    {
                            int c;
                            for ( ; ; ) {
                                while ((c = yyinput()) != '*' && c != EOF && c != 0);
                                if (c == '*') {
                                    while ((c = yyinput()) == '*');
                                    if ( c == '/' ) break;
                                }
                                if (c == EOF || c == 0) {
                                    yyerror("EOF in comment");
                                    break;
                                }
                            }
                        }
\"(\\.|[^\"])*\"        return yy::parser::make_STRING(string(yytext));
[-()<>=+*/%;{.]         return *yytext;
"}"                     {
    yy_push_state(BLOCK_END);
    return *yytext;
}
"<<"                    return yy::parser::token::SHL;
">>"                    return yy::parser::token::SHR;
">="                    return yy::parser::token::GE;
"<="                    return yy::parser::token::LE;
"=="                    return yy::parser::token::EQ;
"!="                    return yy::parser::token::NE;
"+="                    return yy::parser::token::ADDAS;
"-="                    return yy::parser::token::MINAS;
"*="                    return yy::parser::token::MULAS;
"/="                    return yy::parser::token::DIVAS;
"%="                    return yy::parser::token::REMAS;
"<<="                   return yy::parser::token::SHLAS;
">>="                   return yy::parser::token::SHRAS;
[ \t\r]+                ;       /* ignore whitespace */
<BLOCK_END>\n           {
    if (!Lexer::isCompiler) {
        return ';';
    }
}
\n                      ;
\/\/.*$                 ;       /* ignore line comment */
.                       yyerror(string("Unknown character"));
%%

int yywrap(void) {
    return 1;
}
