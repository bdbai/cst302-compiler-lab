#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <cmath>
#include <iomanip>
#include "context.h"
#include "common.h"

void before_processing();
void after_processing();
variant<int32_t, double, string>
convertType(const variant<int32_t, double, string> &from,
            const string &typeFrom, const string &typeTo);
void exAssign(const operatorNode &opr);
double variantToDouble(const variant<int32_t, double, string> &v);
variant<int32_t, double, string> exBin(const operatorNode &opr);
void ex(nodeType &p);
variant<int32_t, double, string> interpretUminus(const nodeType &p);
template <typename T>
T interpretBinOpr(int operatorToken, const T opr1, const T opr2);
void interpretWhile(const operatorNode &p);
bool isTruthy(const variant<int32_t, double, string> &val);
void interpretIf(const operatorNode &p);
string string_replace(const string &haystack, const string &needle,
                      const std::string &replaceStr);
string unquote(const string &s);
void interpretPrint(const operatorNode &p);
optional<variant<int32_t, double, string>> interpret(nodeType &p);

#endif