MAKEFLAGS += -rR
LEX=lex
YACC=yacc
CC=g++
CFLAGS=-Wall -Wextra -std=c++17
NAME=calculator
SOURCES=lexer.cc method.cc nodeType.cc context.cc func.cc
LEX_FILE=$(NAME).l
YACC_FILE=$(NAME).y
LEX_GENERATED_C=lex.yy.cc
YACC_GENERATED_NO_TAB=y
YACC_GENERATED=$(YACC_GENERATED_NO_TAB).tab
YACC_GENERATED_C=$(YACC_GENERATED).cc
YACC_GENERATED_H=$(YACC_GENERATED).h
FILE_INTERPRETER=interpreter
FILE_COMPILER=compiler
FILE_GRAPH=graph

build: $(FILE_INTERPRETER) $(FILE_COMPILER) $(FILE_GRAPH)

%.out: $(LEX_GENERATED_C) $(YACC_GENERATED_C) %.cc
	$(CC) $(CFLAGS) -g -o $@ $^ $(SOURCES) -lfl

$(LEX_GENERATED_C): $(LEX_FILE)
	$(LEX) -v -o $@ $^

$(YACC_GENERATED_C): $(YACC_FILE)
	$(YACC) -v -Wno-yacc -d -o $@ $^

$(FILE_INTERPRETER): $(FILE_INTERPRETER).out

$(FILE_COMPILER): $(FILE_COMPILER).out

$(FILE_GRAPH): $(FILE_GRAPH).out

all: build

.PHONY: clean all

clean:
	rm -f *.out *.out.exe $(LEX_GENERATED_C) $(YACC_GENERATED)* $(YACC_GENERATED_NO_TAB).output
