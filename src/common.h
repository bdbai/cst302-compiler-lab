#ifndef COMMON_H
#define COMMON_H
#include <memory>
#include <variant>
#include <iostream>
#include <unordered_map>
#include <map>
using namespace std;
#include "nodeType.h"
#include "y.tab.hh"

typedef yy::parser::token_type token;
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

const unordered_map<int, const char *> tokenToOperator = {
    {'+', "add"},       {'-', "sub"},       {'*', "mul"}, {'/', "div"},
    {'<', "clt"},       {token::GE, "clt"}, {'>', "cgt"}, {token::LE, "cgt"},
    {token::EQ, "ceq"}, {token::NE, "ceq"}};
map<string, symbol> symbols;

void before_processing();
void after_processing();
void ex(const nodeType &p);
#endif
