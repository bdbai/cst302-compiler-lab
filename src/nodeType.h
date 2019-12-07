#ifndef NODE_TYPE_H
#define NODE_TYPE_H
#include <variant>
#include <memory>
#include <tuple>
#include <optional>
#include <vector>

using namespace std;

class symbol {
  public:
    string literal;
    int ilid;
    string type;
    optional<variant<int32_t, double>> value;
};

class constantNode {
  public:
    constantNode(int32_t con) : innerValue(con) {}
    constantNode(double con) : innerValue(con) {}
    variant<int32_t, double> innerValue;
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

class nodeType {
  public:
    nodeType(constantNode con) : innerNode(con) {}
    nodeType(operatorNode op) : innerNode(move(op)) {}
    nodeType(symbolNode sym) : innerNode(sym) {}
    nodeType(vector<unique_ptr<nodeType>> nodes) : innerNode(move(nodes)) {}
    variant<constantNode, operatorNode, symbolNode,
            vector<unique_ptr<nodeType>>>
        innerNode;
    static unique_ptr<nodeType> make_constant(int32_t con);
    static unique_ptr<nodeType> make_constant(double con);
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
    void push_op(unique_ptr<nodeType> op);
};

#endif
