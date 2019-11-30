## About yylineno

Using the example given in the material (See `line_numbers.l`), one may find that the output starts with 2, not 1. This happens because our lex implementation, `flex`, provides a built-in `yylineno` global variable with an initial value `1`. It can be checked in the generated `lex.yy.c` file around line 332.

```c
extern int yylineno;
int yylineno = 1;
```

There are two ways to workaround this.

### Use our own variable
We define and use our own `lineno` instead of `yylineno`. See `line_numbers_impl.l`.

### Manage `yylineno` correctly
Since `yylineno` is initialed by value `1`, it should be used to present the number of the **current** line (not the next one). Therefore, we take the value before it is increased. See `line_numbers_builtin.l`.

```diff
< ^(.*)\n    printf("%4d\t%s", yylineno++, yytext);
---
> ^(.*)\n    printf("%4d\t%s", ++yylineno, yytext);
```
