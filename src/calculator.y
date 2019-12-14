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
    #include "func.h"
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
void exFunc(shared_ptr<func> f);
void after_processing();
void yyerror(char *s);
%}

%token <int32_t> INTEGER
%token <double> DECIMAL
%token <string> STRING
%token <string> VARIABLE
%token WHILE IF PRINT FOR BREAK CONTINUE FN RETURN
%token '(' ')' '{' '}' ';' ','
%nonassoc IFX
%nonassoc ELSE

%nonassoc '=' ADDAS MINAS MULAS DIVAS REMAS SHLAS SHRAS
%nonassoc NEW
%left AND OR
%nonassoc '!'
%left GE LE EQ NE '>' '<'
%left SHL SHR
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <unique_ptr<nodeType>> stmt expr stmt_list stmt_list_opt stmt_expr stmt_expr_opt expr_opt
%type <vector<unique_ptr<nodeType>>> param_list param_list_opt
%type <optional<unique_ptr<nodeType>>> if_tail
%type <symbol> fn_param
%type <vector<symbol>> fn_param_list fn_param_list_opt

%code {
    using namespace yy;
}
%%

program:
        function                { after_processing(); }
        ;

function:
          function stmt         { ex(*$2); }
        | function FN VARIABLE '(' fn_param_list_opt ')' VARIABLE '{' stmt_list_opt '}' {
          exFunc(func::make_func($3, $5, $7, move($9)));
        }
        | function FN VARIABLE '(' fn_param_list_opt ')' '{' stmt_list_opt '}' {
          exFunc(func::make_func($3, $5, "void", move($8)));
        }
        | /* NULL */
        ;

fn_param_list_opt:
          fn_param_list         { $$ = $1; }
        | /* NULL */            { $$ = {}; }
        ;

fn_param_list:
          fn_param              { $$ = { $1 }; }
        | fn_param_list ',' fn_param {
          $3.ilid = $1.size();
          $1.push_back($3);
          $$ = $1;
        }
        ;

fn_param:
          VARIABLE VARIABLE     { $$ = func::make_param($2, $1); }

stmt_expr_opt:
          stmt_expr             { $$ = move($1); }
        | /* NULL */            { $$ = nodeType::make_ops(); }
        ;

expr_opt:
          expr                  { $$ = move($1); }
        | /* NULL */            { $$ = nodeType::make_ops(); }
        ;

stmt:
          ';'                            { $$ = nodeType::make_ops(); }
        | CONTINUE ';'                   { $$ = nodeType::make_op(token::CONTINUE); }
        | RETURN expr ';'                { $$ = nodeType::make_op(token::RETURN, move($2)); }
        | RETURN ';'                     { $$ = nodeType::make_op(token::RETURN); }
        | BREAK ';'                      { $$ = nodeType::make_op(token::BREAK); }
        | stmt_expr ';'                  { $$ = move($1); }
        | WHILE '(' expr ')' stmt        { $$ = nodeType::make_op(token::WHILE, move($3), move($5)); }
        | IF '(' expr ')' stmt           { closingIf = true; }
          if_tail                        {
            closingIf = false;
            if ($7.has_value()) {
              $$ = nodeType::make_op(token::IF, move($3), move($5), move($7.value()));
            } else {
              $$ = nodeType::make_op(token::IF, move($3), move($5));
            }
          }
        | FOR '(' stmt_expr_opt ';' expr_opt ';' stmt_expr_opt ')' stmt {
          auto whileNode = nodeType::make_op(token::WHILE, move($5), move($9));
          $$ = nodeType::make_op(token::FOR, move($3), move(whileNode), move($7));
        }
        | '{' stmt_list_opt '}'              { $$ = move($2); }
        ;

stmt_expr:
        expr                    { $$ = move($1); }
        | PRINT expr            { $$ = nodeType::make_op(token::PRINT, move($2)); }
        | VARIABLE '=' expr     { $$ = nodeType::make_op('=', move(nodeType::make_symbol($1)), move($3)); }
        | VARIABLE ADDAS expr   { $$ = nodeType::make_opas('+', $1, move($3)); }
        | VARIABLE MINAS expr   { $$ = nodeType::make_opas('-', $1, move($3)); }
        | VARIABLE MULAS expr   { $$ = nodeType::make_opas('*', $1, move($3)); }
        | VARIABLE DIVAS expr   { $$ = nodeType::make_opas('/', $1, move($3)); }
        | VARIABLE REMAS expr   { $$ = nodeType::make_opas('%', $1, move($3)); }
        | VARIABLE SHLAS expr   { $$ = nodeType::make_opas(token::SHL, $1, move($3)); }
        | VARIABLE SHRAS expr   { $$ = nodeType::make_opas(token::SHR, $1, move($3)); }
        ;

if_tail:
          %prec IFX                     { $$ = {}; }
        | ELSE stmt                     { $$ = move($2); }

stmt_list_opt:
          stmt_list             { $$ = move($1); }
        | /* NULL */            { $$ = nodeType::make_ops();}

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
        | '!' expr %prec EQ     { $$ = nodeType::make_op(token::EQ, move($2), nodeType::make_constant(0)); }
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
        | expr AND expr         { $$ = nodeType::make_op(token::AND, nodeType::make_op(token::NE, move($1), nodeType::make_constant(0)), nodeType::make_op(token::NE, move($3), nodeType::make_constant(0))); }
        | expr OR expr          { $$ = nodeType::make_op(token::OR, nodeType::make_op(token::EQ, move($1), nodeType::make_constant(1)), nodeType::make_op(token::EQ, move($3), nodeType::make_constant(1))); }
        | '(' expr ')'          { $$ = move($2); }
        | NEW VARIABLE '(' param_list_opt ')' { $$ = nodeType::make_ctor($2, move($4)); }
        | VARIABLE '(' param_list_opt ')'     { $$ = nodeType::make_call($1, move($3)); }
        ;

param_list_opt:
        param_list              { $$ = move($1); }
        | /* NULL */            { $$ = move(vector<unique_ptr<nodeType>>()); }
        ;

param_list:
        param_list ',' expr     { $1.push_back(move($3)); $$ = move($1); }
        | expr                  {
          auto l = vector<unique_ptr<nodeType>>();
          l.push_back(move($1));
          $$ = move(l);
        }
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
