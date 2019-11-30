MAKEFLAGS += -rR
LEX=lex
CC=gcc
CFLAGS=-Wall -Wextra
NAME=line_numbers
FILE=$(NAME)
FILE_IMPL=$(NAME)_impl
FILE_BUILTIN=$(NAME)_builtin

%.out: %.lex.yy.c 
	$(CC) $(CFLAGS) -o $@ $^

%.lex.yy.c: %.l
	$(LEX) -v -o $@ $^

all: $(FILE).out $(FILE_IMPL).out $(FILE_BUILTIN).out

.PHONY: clean all

clean:
	rm -f *.out *.out.exe *.lex.yy.c
