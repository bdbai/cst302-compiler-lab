#include "interpreter.h"

void before_processing() {}

void after_processing() {}

void exAssign(const operatorNode &opr) {
    const auto &[id_ptr, expr_ptr] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    const auto &id = *id_ptr;
    const auto &expr = *expr_ptr;
    const auto &variableNode = get<symbolNode>(id.innerNode);
    if (symbols.find(variableNode.symbol) == symbols.end()) {
        // Declare a new variable
        symbol sym;
        sym.literal = variableNode.symbol;
        sym.ilid = symbols.size();
        sym.type = "int32"; // TODO: type inference
        symbols[variableNode.symbol] = sym;
    }
    auto &sym = symbols[variableNode.symbol];
    sym.value = interpret(expr);
}

template <typename T>
T interpretBinOpr(int operatorToken, const T opr1, const T opr2) {
    switch (operatorToken) {
    case '+':
        return opr1 + opr2;
    case '-':
        return opr1 - opr2;
    case '*':
        return opr1 * opr2;
    case '/':
        return opr1 / opr2;
    case '<':
        return opr1 < opr2;
    case token::GE:
        return opr1 >= opr2;
    case '>':
        return opr1 > opr2;
    case token::LE:
        return opr1 <= opr2;
    case token::EQ:
        return opr1 == opr2;
    case token::NE:
        return opr1 != opr2;
    default:
        cerr << "Unknown operator type: " << (char)operatorToken << endl;
        abort();
        return T();
    }
}

variant<int32_t, double> exBin(const operatorNode &opr) {
    if (tokenToOperator.find(opr.operatorToken) == tokenToOperator.end()) {
        cerr << "Cannot find opr" << endl;
        abort();
    }
    const auto &[oprNode1, oprNode2] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    const auto opr1 = interpret(*oprNode1).value();
    const auto opr2 = interpret(*oprNode2).value();
    if (opr1.index() != opr2.index()) {
        cerr << "Type mismatch" << endl;
        abort();
    }
    if (holds_alternative<double>(opr1) && holds_alternative<double>(opr2)) {
        return interpretBinOpr(opr.operatorToken, get<double>(opr1),
                               get<double>(opr2));
    } else {
        return interpretBinOpr(opr.operatorToken, get<int32_t>(opr1),
                               get<int32_t>(opr2));
    }
}

void ex(const nodeType &p) { interpret(p); }

variant<int32_t, double> interpretUminus(const operatorNode &p) {
    const auto &oprand =
        interpret(*get<unique_ptr<nodeType>>(p.operands.value())).value();
    return visit(
        overloaded{[](int32_t val) -> variant<int32_t, double> { return -val; },
                   [](double val) -> variant<int32_t, double> { return -val; }},
        oprand);
}
void interpretWhile(const operatorNode &p) {
    const auto &operands =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            p.operands.value());
    int32_t cond;
    while ((cond = get<int32_t>(interpret(*operands.first).value()))) {
        interpret(*operands.second);
    }
}
void interpretIf(const operatorNode &p) {
    visit(
        overloaded{
            [](const unique_ptr<nodeType> &body) {
                cerr << "if statement has no operands" << endl;
                abort();
            },
            [](const pair<unique_ptr<nodeType>, unique_ptr<nodeType>> &ifBody) {
                const auto &[condNodePtr, bodyNodePtr] = ifBody;
                if (interpret(*condNodePtr)) {
                    interpret(*bodyNodePtr);
                }
            },
            [](const tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                           unique_ptr<nodeType>> &ifElseBody) {
                const auto &[condNodePtr, bodyNodePtr, elseNodePtr] =
                    ifElseBody;
                if (interpret(*condNodePtr)) {
                    interpret(*bodyNodePtr);
                } else {
                    interpret(*elseNodePtr);
                }
            }},
        p.operands.value());
}
void interpretPrint(const operatorNode &p) {
    const auto &oprand =
        interpret(*get<unique_ptr<nodeType>>(p.operands.value())).value();
    visit(overloaded{[](const double val) { cout << val << endl; },
                     [](const int32_t val) { cout << val << endl; }},
          oprand);
}

optional<variant<int32_t, double>> interpret(const nodeType &p) {
    return visit(
        overloaded{[](const constantNode &conNode)
                       -> optional<variant<int32_t, double>> {
                       return conNode.innerValue;
                   },
                   [](const operatorNode &oprNode)
                       -> optional<variant<int32_t, double>> {
                       switch (oprNode.operatorToken) {
                       case token::WHILE:
                           interpretWhile(oprNode);
                           break;
                       case token::IF:
                           interpretIf(oprNode);
                           break;
                       case token::UMINUS:
                           return interpretUminus(oprNode);
                       case '=':
                           exAssign(oprNode);
                           break;
                       case token::PRINT:
                           interpretPrint(oprNode);
                           // TODO: other types
                           break;
                       default:
                           // Deal with binary operators
                           return exBin(oprNode);
                       }
                       return {};
                   },
                   [](const symbolNode &symNode)
                       -> optional<variant<int32_t, double>> {
                       if (symbols.find(symNode.symbol) == symbols.end()) {
                           cerr << "Undefined variable: " << symNode.symbol
                                << endl;
                           abort();
                           return {};
                       }
                       const auto &sym = symbols[symNode.symbol];
                       return sym.value;
                   },
                   [](const vector<unique_ptr<nodeType>> &nodes)
                       -> optional<variant<int32_t, double>> {
                       for (const auto &node : nodes) {
                           interpret(*node);
                       }
                       return {};
                   },
                   [](auto &rest) -> optional<variant<int32_t, double>> {
                       abort();
                       return {};
                   }},
        p.innerNode);
}
