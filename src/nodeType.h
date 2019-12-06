#ifndef NODE_TYPE_H
#define NODE_TYPE_H
#include <variant>
#include <memory>
#include <tuple>
#include <optional>

using namespace std;

class symbol {
  public:
    string literal;
    int ilid;
    string type;
};

class constantNode {
  public:
    constantNode(int32_t con) : innerValue(con) {}
    constantNode(double con) : innerValue(con) {}
    std::variant<int32_t, double> innerValue;
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
    variant<constantNode, operatorNode, symbolNode> innerNode;
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
};

#endif
