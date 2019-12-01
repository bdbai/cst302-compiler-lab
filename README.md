# cst302-compiler-lab

## Windows Subsystem for Linux (Ubuntu)
The lex and yacc implementation we are going to use, `flex` and `yacc`, is much more easier to install on a GNU/Linux environment. For those using Windows 10, please install Windows Subsystem for Linux (WSL) with a Linux distro by following the [Installation Guide](https://docs.microsoft.com/en-us/windows/wsl/install-win10) and [Initializing Distro](https://docs.microsoft.com/en-us/windows/wsl/initialize-distro). Unless otherwise specified, all commands in this repository are assumed to be executed on Ubuntu.

Once the environment is ready, we may install the tools to build everything.

```bash
sudo apt update # Update available software list
sudo apt install build-essential flex bison # Install compilers, build tools and generators
```

For your information, the local disks can be accessed under `/mnt`.

## How to use `Makefile`s
Each project comes with a `Makefile` that speeds up the build process. Follow the instructions to use `make` on GNU/Linux:

1. Enter the project directory, e.g. `cd practices/lex/echo`;
2. Run `make`;
3. There will be one or more `<project>.out` (e.g. `echo.out`) appears in the directory if the project builds successfully. Run `./<project>.out` to execute;
4. Run `make clean` to clear all generated files. Be cautious not to commit any generated files to the repository.
