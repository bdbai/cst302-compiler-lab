#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <sstream>
#include <stack>
#include "common.h"
#include "context.h"

stringstream ilbuf;
size_t label = 0;
size_t maxStack = 1;
size_t currentStack = 0;
stack<size_t> continueJump;
stack<optional<size_t>> breakJump;

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
void exContinue();
void exBreak();
void ex(nodeType &p, Context ctx);
#endif
