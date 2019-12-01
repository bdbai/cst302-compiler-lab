
LEX=flex
YACC=bison
CC=gcc
CFLAGS=-Wall -Wextra
NAME=calculator
LEX_FILE=$(NAME).l
YACC_FILE=$(NAME).y
LEX_GENERATED_C=lex.yy.c
YACC_GENERATED_NO_TAB=y
YACC_GENERATED=$(YACC_GENERATED_NO_TAB).tab
YACC_GENERATED_C=$(YACC_GENERATED).c
YACC_GENERATED_H=$(YACC_GENERATED).h
FILE_INTEPRETER=intepreter
FILE_COMPILER=compiler
FILE_GRAPH=graph

build: $(FILE_INTEPRETER) $(FILE_COMPILER) $(FILE_GRAPH)

%.out: $(LEX_GENERATED_C) $(YACC_GENERATED_C) %.c
	@echo compilingggggggggggggggg
	$(CC) $(CFLAGS) -o $@ $^
	@echo compilingggggdddddddddddddddddddd

$(LEX_GENERATED_C): $(LEX_FILE)
	$(LEX) -v -o"$@" $^

$(YACC_GENERATED_C): $(YACC_FILE)
	$(YACC) -v -d -o $@ $^

$(FILE_INTEPRETER): $(FILE_INTEPRETER).out

$(FILE_COMPILER): $(FILE_COMPILER).out

$(FILE_GRAPH): $(FILE_GRAPH).out

all: build

.PHONY: clean all

clean:
	del *.out *.out.exe $(LEX_GENERATED_C) $(YACC_GENERATED)* $(YACC_GENERATED_NO_TAB).output
