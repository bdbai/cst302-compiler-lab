MAKEFLAGS += -rR
LEX=lex
CC=gcc
CFLAGS=-Wall -Wextra
NAME=echo
LEX_FILE=$(NAME).l
LEX_GENERATED_C=lex.yy.c
OUTPUT_FILE=$(NAME).out

$(OUTPUT_FILE): $(LEX_GENERATED_C)
	$(CC) $(CFLAGS) -o $@ $^

$(LEX_GENERATED_C): $(LEX_FILE)
	$(LEX) -v -o $@ $^

all: $(OUTPUT_FILE)

.PHONY: clean all

clean:
	rm -f $(OUTPUT_FILE) $(OUTPUT_FILE).exe $(LEX_GENERATED_C)
