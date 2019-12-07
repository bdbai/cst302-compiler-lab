#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "context.h"
#include "common.h"

void before_processing();
void after_processing();
void exAssign(const operatorNode &opr);

double variantToDouble(const variant<int32_t, double> &v);
variant<int32_t, double> exBin(const operatorNode &opr);
void ex(nodeType &p);
variant<int32_t, double> interpretUminus(const nodeType &p);
template <typename T>
T interpretBinOpr(int operatorToken, const T opr1, const T opr2);
void interpretWhile(const operatorNode &p);
void interpretIf(const operatorNode &p);
void interpretPrint(const operatorNode &p);
optional<variant<int32_t, double>> interpret(nodeType &p);

#endif