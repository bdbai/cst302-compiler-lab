## About yylineno

Using the example given in the material (See `line_numbers.l`), one may find that the output starts with 2, not 1. This happens because our lex implementation, `flex`, provides a built-in `yylineno` global variable with an initial value `1`. It can be checked in the generated `lex.yy.c` file around line 332.

```c
extern int yylineno;
int yylineno = 1;
```

There are two ways to workaround this.

### Use our own variable
We define and use our own `lineno` instead of `yylineno`. See `line_numbers_impl.l`.

### The `yylineno` option
`flex` has an built-in option called `yylineno`, which will update `yylineno` automatically. Simply appending `--yylineno` to the `lex` command line options or adding `%option yylineno` at the beginning of the our `.l` file will do the trick. However, this does not solve the problem.

`yylineno` starts from 1 and increases upon the first `\n` (new line), which will in turn become 2. To solve it, we separate `\n` (new line) and `$` (line end) so that the line number will be read before its increment. See `line_numbers_builtin.l`.
