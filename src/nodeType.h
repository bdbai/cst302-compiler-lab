#ifndef NODE_TYPE_H
#define NODE_TYPE_H
#include <variant>
#include <memory>
#include <tuple>
#include <optional>
#include <vector>
#include <iostream>
#include <map>
#include <cstdint>
#include "method.h"

using namespace std;

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

class symbol {
  public:
    string literal;
    int ilid;
    string type;
    optional<variant<int32_t, double, string>> value;
};

extern map<string, symbol> symbols;

class constantNode {
  public:
    constantNode(int32_t con) : innerValue(con) {}
    constantNode(double con) : innerValue(con) {}
    constantNode(string con) : innerValue(con) {}
    variant<int32_t, double, string> innerValue;
};

class symbolNode {
  public:
    symbolNode(string sym) : symbol(sym) {}
    string symbol;
    bool operator==(const symbolNode &other) const;
    bool operator<(const symbolNode &other) const;
};

class nodeType;

class operatorNode {
  public:
    // yy::parser::token_type operatorToken;
    // avoid circular reference
    int operatorToken;
    optional<variant<unique_ptr<nodeType>,
                     pair<unique_ptr<nodeType>, unique_ptr<nodeType>>,
                     tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                           unique_ptr<nodeType>>>>
        operands;
};

class callNode {
  private:
    optional<reference_wrapper<const method>> resolvedMethod;

  public:
    callNode(const string &func, vector<unique_ptr<nodeType>> params)
        : func(func), params(move(params)) {}
    string func;
    vector<unique_ptr<nodeType>> params;
    optional<reference_wrapper<const method>> resolveMethod();
};

class nodeType {
  private:
    optional<string> type;

  public:
    nodeType(constantNode con, const string &type)
        : type(type), innerNode(con) {}
    nodeType(operatorNode op) : innerNode(move(op)) {}
    nodeType(symbolNode sym) : innerNode(sym) {}
    nodeType(callNode call) : innerNode(move(call)) {}
    nodeType(vector<unique_ptr<nodeType>> nodes) : innerNode(move(nodes)) {}
    variant<constantNode, operatorNode, symbolNode,
            vector<unique_ptr<nodeType>>, callNode>
        innerNode;
    static unique_ptr<nodeType> make_constant(const int32_t con);
    static unique_ptr<nodeType> make_constant(const double con);
    static unique_ptr<nodeType> make_constant(const string &con);
    static unique_ptr<nodeType> make_symbol(string sym);
    static unique_ptr<nodeType> make_op(int opr);
    static unique_ptr<nodeType> make_op(int opr, unique_ptr<nodeType> arg1);
    static unique_ptr<nodeType> make_op(int opr, unique_ptr<nodeType> arg1,
                                        unique_ptr<nodeType> arg2);
    static unique_ptr<nodeType> make_op(int opr, unique_ptr<nodeType> arg1,
                                        unique_ptr<nodeType> arg2,
                                        unique_ptr<nodeType> arg3);
    static unique_ptr<nodeType> make_ops();
    static unique_ptr<nodeType> make_ops(unique_ptr<nodeType> op);
    static unique_ptr<nodeType> make_opas(int opr, const string symbol,
                                          unique_ptr<nodeType> arg);
    static unique_ptr<nodeType> make_call(const string &func,
                                          vector<unique_ptr<nodeType>> params);
    void push_op(unique_ptr<nodeType> op);
    const string inferType();
    void setType(const string &type);
};

const string getTypeCommon(const string &type1, const string &type2);

#endif
