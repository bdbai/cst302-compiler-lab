# Complex Calculator

## Components
### `intepreter`
> an interpreter that executes statements during the tree walk
### `compiler`
> a compiler that generates code for a *hypothetical stack-based machine*
### `graph`
> a version that generates a syntax tree of the original program

## How to build
- Run `mingw32-make` to build all 3 components;
- Run `mingw32-make <component>` to build a single component, e.g. `mingw32-make intepreter` will only build the intepreter.
