#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <sstream>
#include "common.h"

stringstream ilbuf;
size_t label = 0;
size_t maxStack = 0;
size_t currentStack = 0;

void exAssign(const operatorNode &opr);
void exBin(const operatorNode &opr);
void exWhile(const operatorNode &p);
void exIf(const operatorNode &p);
#endif
