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
[-()<>=+*/;{}.]         return *yytext;
">="                    return yy::parser::token::GE;
"<="                    return yy::parser::token::LE;
"=="                    return yy::parser::token::EQ;
"!="                    return yy::parser::token::NE;
[ \t\n]+                ;       /* ignore whitespace */
\/\/.*$                 ;       /* ignore line comment */
.                       yyerror(string("Unknown character"));
%%

int yywrap(void) {
    return 1;
}
