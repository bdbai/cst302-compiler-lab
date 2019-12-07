#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "common.h"

void before_processing();
void after_processing();
void exAssign(const operatorNode &opr);

variant<int32_t, double> exBin(const operatorNode &opr);
void ex(const nodeType &p);
variant<int32_t, double> interpretUminus(const nodeType &p);
template <typename T>
T interpretBinOpr(int operatorToken, const T opr1, const T opr2);
void interpretWhile(const operatorNode &p);
void interpretIf(const operatorNode &p);
void interpretPrint(const operatorNode &p);
optional<variant<int32_t, double>> interpret(const nodeType &p);

#endif