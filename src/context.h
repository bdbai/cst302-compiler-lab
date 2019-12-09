#ifndef CONTEXT_H
#define CONTEXT_H
#include <string>
#include <iostream>
using namespace std;

enum ExpectedType { None, Integer, Decimal, String };
class Context {
  public:
    ExpectedType expecting = ExpectedType::None;
    static ExpectedType typeStringToExpected(const string &);
};
#endif