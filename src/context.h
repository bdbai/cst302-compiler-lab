#ifndef CONTEXT_H
#define CONTEXT_H
#include <string>
#include <sstream>
#include <iostream>
#include <stack>
#include <optional>
#include <memory>
using namespace std;

enum ExpectedType { None, Any, Integer, Decimal, String };
class func;
class Context {
  public:
    Context(shared_ptr<func> currentFunc) : currentFunc(currentFunc) {}
    stringstream ilbuf;
    size_t maxStack = 1;
    size_t currentStack = 0;
    stack<size_t> continueJump;
    stack<optional<size_t>> breakJump;
    shared_ptr<func> currentFunc;
    static ExpectedType typeStringToExpected(const string &);
};
#endif