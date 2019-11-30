%option yylineno
%%
^(.*)$    printf("%4d\t%s\n", yylineno, yytext);
\n        {}
%%

int yywrap(void) {
    return 1;
}

int main(int argc, char *argv[]) {
    yyin = fopen(argv[1], "r");
    yylex();
    fclose(yyin);
}
