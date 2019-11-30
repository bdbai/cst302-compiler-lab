# cst302-compiler-lab

## How to use `Makefile`s
Each project comes with a `Makefile` that speeds up the build process. Follow the instructions to use `make`:

1. Ensure `<code_blocks_install_dir>\MinGW\bin` is in `PATH` (most of you have done this);
2. Open Command Prompt or PowerShell and enter the project directory, e.g. `cd practices\lex\echo`;
3. Run `mingw32-make`;
4. There will be one or more `<project>.out.exe` (e.g. `echo.out.exe`) appears in the directory if the project builds successfully. Run `echo.out` to execute;
5. Run `mingw32-make clean` to clear all generated files. Be cautious not to commit any generated files to the repository.
