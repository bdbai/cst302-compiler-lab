#ifndef COMMON_H
#define COMMON_H
#include <memory>
#include <variant>
#include <iostream>
#include <unordered_map>
using namespace std;
#include "nodeType.h"
#include "y.tab.hh"
#include "lexer.h"

typedef yy::parser::token_type token;

const unordered_map<int, const char *> tokenToOperator = {
    {'+', "add"},        {'-', "sub"},       {'*', "mul"},
    {'/', "div"},        {'<', "clt"},       {'%', "rem"},
    {token::AND, "and"}, {token::OR, "or"},  {token::SHL, "shl"},
    {token::SHR, "shr"}, {token::GE, "clt"}, {'>', "cgt"},
    {token::LE, "cgt"},  {token::EQ, "ceq"}, {token::NE, "ceq"}};

void before_processing();
void after_processing();
void ex(nodeType &p);
void exFunc(shared_ptr<func> f);
#endif
