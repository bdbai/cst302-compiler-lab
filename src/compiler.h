#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <sstream>
#include "common.h"
#include "context.h"

stringstream ilbuf;
size_t label = 0;
size_t maxStack = 0;
size_t currentStack = 0;

void convertType(const string &typeFrom, const string &typeTo);
void convertType(const string &typeFrom, const ExpectedType expecting);
void exAssign(const operatorNode &opr);
void exBin(const operatorNode &opr);
void exWhile(const operatorNode &p);
void exIf(const operatorNode &p);
void exPrint(const operatorNode &p);
void ex(nodeType &p, Context ctx);
#endif
