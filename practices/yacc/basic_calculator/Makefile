MAKEFLAGS += -rR
LEX=lex
YACC=yacc
CC=gcc
CFLAGS=-Wall -Wextra
NAME=basic_calculator
LEX_FILE=$(NAME).l
YACC_FILE=$(NAME).y
LEX_GENERATED_C=lex.yy.c
YACC_GENERATED_NO_TAB=y
YACC_GENERATED=$(YACC_GENERATED_NO_TAB).tab
YACC_GENERATED_C=$(YACC_GENERATED).c
YACC_GENERATED_H=$(YACC_GENERATED).h
OUTPUT_FILE=$(NAME).out

$(OUTPUT_FILE): $(LEX_GENERATED_C) $(YACC_GENERATED_C)
	$(CC) $(CFLAGS) -o $@ $^

$(LEX_GENERATED_C): $(LEX_FILE)
	$(LEX) -v -o $@ $^

$(YACC_GENERATED_C): $(YACC_FILE)
	$(YACC) -v -d -o $@ $^

all: $(OUTPUT_FILE)

.PHONY: clean all

clean:
	rm -f $(OUTPUT_FILE) $(OUTPUT_FILE).exe $(LEX_GENERATED_C) $(YACC_GENERATED)* $(YACC_GENERATED_NO_TAB).output
