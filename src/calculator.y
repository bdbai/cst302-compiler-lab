%require "3.2"
%skeleton "lalr1.cc"
%language "c++"
%define api.token.constructor
%define api.value.type variant
// %define api.value.automove
%token END_OF_FILE 0
%code requires {
    #include <memory>
    #include <vector>
    using namespace std;
    #include "nodeType.h"
}
%{
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstdint>
#include <memory>
#include <string>
using namespace std;

#include "lexer.h"
Lexer lexer;
bool closingIf;

extern "C"
yy::parser::symbol_type yylex() {
    return lexer.yylex(0);
}

/* prototypes */
void before_processing();
int ex(nodeType &p);
void after_processing();
void yyerror(char *s);
%}

%token <int32_t> INTEGER
%token <double> DECIMAL
%token <string> STRING
%token <string> VARIABLE
%token WHILE IF PRINT
%token '(' ')' '{' '}' ';'
%nonassoc IFX
%nonassoc ELSE

%nonassoc '=' ADDAS MINAS MULAS DIVAS REMAS SHLAS SHRAS
%left GE LE EQ NE '>' '<'
%left SHL SHR
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <unique_ptr<nodeType>> stmt expr stmt_list if_tail

%code {
    using namespace yy;
}
%%

program:
        function                { after_processing(); }
        ;

function:
          function stmt         { ex(*$2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = nodeType::make_ops(); }
        | expr ';'                       { $$ = move($1); }
        | PRINT expr ';'                 { $$ = nodeType::make_op(token::PRINT, move($2)); }
        | VARIABLE '=' expr ';'          { $$ = nodeType::make_op('=', move(nodeType::make_symbol($1)), move($3)); }
        | VARIABLE ADDAS expr ';'        { $$ = nodeType::make_opas('+', $1, move($3)); }
        | VARIABLE MINAS expr ';'        { $$ = nodeType::make_opas('-', $1, move($3)); }
        | VARIABLE MULAS expr ';'        { $$ = nodeType::make_opas('*', $1, move($3)); }
        | VARIABLE DIVAS expr ';'        { $$ = nodeType::make_opas('/', $1, move($3)); }
        | VARIABLE REMAS expr ';'        { $$ = nodeType::make_opas('%', $1, move($3)); }
        | VARIABLE SHLAS expr ';'        { $$ = nodeType::make_opas(token::SHL, $1, move($3)); }
        | VARIABLE SHRAS expr ';'        { $$ = nodeType::make_opas(token::SHR, $1, move($3)); }
        | WHILE '(' expr ')' stmt        { $$ = nodeType::make_op(token::WHILE, move($3), move($5)); }
        | IF '(' expr ')' stmt           { closingIf = true; }
          if_tail                        { closingIf = false; $$ = nodeType::make_op(token::IF, move($3), move($5), move($7)); }
        | '{' stmt_list '}'              { $$ = move($2); }
        ;

if_tail:
          %prec IFX                     { $$ = nodeType::make_ops(); }
        | ELSE stmt                     { $$ = move($2); }
        

stmt_list:
          stmt                  { $$ = move(nodeType::make_ops(move($1))); }
        | stmt_list stmt        { $1->push_op(move($2)); $$ = move($1); }
        ;

expr:
          INTEGER               { $$ = nodeType::make_constant($1); }
        | STRING                { $$ = nodeType::make_constant($1); }
        | DECIMAL               { $$ = nodeType::make_constant($1); }
        | VARIABLE              { $$ = nodeType::make_symbol($1); }
        | '-' expr %prec UMINUS { $$ = nodeType::make_op(token::UMINUS, move($2)); }
        | expr '+' expr         { $$ = nodeType::make_op('+', move($1), move($3)); }
        | expr '-' expr         { $$ = nodeType::make_op('-', move($1), move($3)); }
        | expr '*' expr         { $$ = nodeType::make_op('*', move($1), move($3)); }
        | expr '/' expr         { $$ = nodeType::make_op('/', move($1), move($3)); }
        | expr '%' expr         { $$ = nodeType::make_op('%', move($1), move($3)); }
        | expr SHL expr         { $$ = nodeType::make_op(token::SHL, move($1), move($3)); }
        | expr SHR expr         { $$ = nodeType::make_op(token::SHR, move($1), move($3)); }
        | expr '<' expr         { $$ = nodeType::make_op('<', move($1), move($3)); }
        | expr '>' expr         { $$ = nodeType::make_op('>', move($1), move($3)); }
        | expr GE expr          { $$ = nodeType::make_op(token::GE, move($1), move($3)); }
        | expr LE expr          { $$ = nodeType::make_op(token::LE, move($1), move($3)); }
        | expr NE expr          { $$ = nodeType::make_op(token::NE, move($1), move($3)); }
        | expr EQ expr          { $$ = nodeType::make_op(token::EQ, move($1), move($3)); }
        | '(' expr ')'          { $$ = move($2); }
        ;

%%

void yyerror(const string &s) {
    cerr << s << endl;
}

int main(void) {
    before_processing();
    yy::parser parser;
    parser.parse();
    //after_processing();
    return 0;
}

void yy::parser::error(const string& message) {
  cerr << message << endl;
}
