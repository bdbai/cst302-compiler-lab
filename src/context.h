#ifndef CONTEXT_H
#define CONTEXT_H
enum ExpectedType { None, Integer, Decimal };
class Context {
  public:
    ExpectedType expecting = ExpectedType::None;
};
#endif