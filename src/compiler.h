#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <list>
#include "common.h"
#include "context.h"

extern Context ctx;
stringstream globalOut;
list<string> funcIls;
size_t label = 0;

void beginFunction(ostream &out);
void endFunction(ostream &out);
void convertType(const string &typeFrom, const string &typeTo);
void convertType(const string &typeFrom, const ExpectedType expecting);
void exAssign(const operatorNode &opr);
void exBin(const operatorNode &opr);
void exLoop(nodeType &condNode, nodeType &bodyNode,
            optional<reference_wrapper<nodeType>> nextNode);
void exWhile(const operatorNode &p);
void exFor(const operatorNode &p);
void exIf(const operatorNode &p);
void exPrint(const operatorNode &p);
void exUminus(const operatorNode &p);
void exContinue();
void exBreak();
void ex(nodeType &p, ExpectedType expecting);
#endif
