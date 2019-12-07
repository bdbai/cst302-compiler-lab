#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <sstream>
#include "common.h"

stringstream ilbuf;
size_t label;

void exAssign(const operatorNode &opr);
void exBin(const operatorNode &opr);
void exWhile(const operatorNode &p);
void exIf(const operatorNode &p);
#endif
