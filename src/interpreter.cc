#include "interpreter.h"

void before_processing() { Lexer::isInterpreter = true; }

void after_processing() {}

void exAssign(const operatorNode &opr) {
    const auto &[id_ptr, expr_ptr] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    const auto &id = *id_ptr;
    auto &expr = *expr_ptr;
    const auto &variableNode = get<symbolNode>(id.innerNode);
    const auto value = interpret(expr);
    const auto symbolIt = symbols.find(variableNode.symbol);
    if (symbolIt == symbols.end()) {
        // Declare a new variable
        symbol sym;
        sym.literal = variableNode.symbol;
        sym.ilid = symbols.size();
        // The `type` field is not used in interpreter since variant is used to
        // check types
        sym.type = "int32";
        symbols[variableNode.symbol] = sym;
    } else {
        if (value.value().index() != symbolIt->second.value.value().index()) {
            cerr << "Type mismatch" << endl;
            abort();
        }
    }
    auto &sym = symbols[variableNode.symbol];
    sym.value = value;
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
    case '%':
        return fmod(opr1, opr2);
    case token::SHL:
        return int32_t(opr1) << int32_t(opr2);
    case token::SHR:
        return int32_t(opr1) >> int32_t(opr2);
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

double variantToDouble(const variant<int32_t, double, string> &v) {
    return visit(
        overloaded{[](const int32_t val) -> double { return (double)val; },
                   [](const double val) -> double { return val; },
                   [](const string) -> double {
                       cerr << "Cannot convert string to double" << endl;
                       abort();
                   }},
        v);
}

variant<int32_t, double, string> exBin(const operatorNode &opr) {
    if (tokenToOperator.find(opr.operatorToken) == tokenToOperator.end()) {
        cerr << "Cannot find opr" << endl;
        abort();
    }
    const auto &[oprNode1, oprNode2] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    const auto opr1 = interpret(*oprNode1).value();
    const auto opr2 = interpret(*oprNode2).value();
    if (holds_alternative<int32_t>(opr1) && holds_alternative<int32_t>(opr2)) {
        return interpretBinOpr(opr.operatorToken, get<int32_t>(opr1),
                               get<int32_t>(opr2));
    } else if (holds_alternative<double>(opr1) ||
               holds_alternative<double>(opr2))

    {
        return interpretBinOpr(opr.operatorToken, variantToDouble(opr1),
                               variantToDouble(opr2));
    } else {
        // TODO: other types
        cerr << "Cannot perform operations because of unknown types" << endl;
        abort();
    }
}

void ex(nodeType &p) { interpret(p); }

variant<int32_t, double, string> interpretUminus(const operatorNode &p) {
    const auto &oprand =
        interpret(*get<unique_ptr<nodeType>>(p.operands.value())).value();
    return visit(
        overloaded{
            [](int32_t val) -> variant<int32_t, double, string> {
                return -val;
            },
            [](double val) -> variant<int32_t, double, string> { return -val; },
            [](string) -> variant<int32_t, double, string> {
                cerr << "Cannot minus string" << endl;
                abort();
            }},
        oprand);
}
bool isTruthy(const variant<int32_t, double, string> &val) {
    return visit(overloaded{[](const int32_t val) -> bool { return (bool)val; },
                            [](const double val) -> bool { return (bool)val; },
                            [](const string) -> bool {
                                cerr << "Cannot use string as conditions"
                                     << endl;
                                abort();
                            }},
                 val);
}
void interpretWhile(const operatorNode &p) {
    const auto &operands =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            p.operands.value());
    while ((isTruthy(interpret(*operands.first).value()))) {
        interpret(*operands.second);
    }
}

void interpretIf(const operatorNode &p) {
    visit(
        overloaded{
            [](const unique_ptr<nodeType> &) {
                cerr << "if statement has no operands" << endl;
                abort();
            },
            [](const pair<unique_ptr<nodeType>, unique_ptr<nodeType>> &ifBody) {
                const auto &[condNodePtr, bodyNodePtr] = ifBody;
                if (isTruthy(interpret(*condNodePtr).value())) {
                    interpret(*bodyNodePtr);
                }
            },
            [](const tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                           unique_ptr<nodeType>> &ifElseBody) {
                const auto &[condNodePtr, bodyNodePtr, elseNodePtr] =
                    ifElseBody;
                if (isTruthy(interpret(*condNodePtr).value())) {
                    interpret(*bodyNodePtr);
                } else {
                    interpret(*elseNodePtr);
                }
            }},
        p.operands.value());
}
void string_replace(string &haystack, const string &needle,
                    const std::string &replaceStr) {
    if (needle.empty()) {
        return;
    }
    size_t pos;
    while ((pos = haystack.find(needle)) != string::npos) {
        haystack.replace(pos, needle.length(), replaceStr);
        pos += replaceStr.length();
    }
}
string unquote(const string &s) {
    static vector<pair<string, string>> patterns = {{"\\\\", "\\"},
                                                    {"\\n", "\n"},
                                                    {"\\r", "\r"},
                                                    {"\\t", "\t"},
                                                    {"\\\"", "\""}};
    string result = s;
    if (result.length() >= 2) {
        // Remove surrounding quotes
        result = result.substr(1, result.length() - 2);
    }
    for (const auto &p : patterns) {
        string_replace(result, p.first, p.second);
    }
    return result;
}
void interpretPrint(const operatorNode &p) {
    const auto operand =
        interpret(*get<unique_ptr<nodeType>>(p.operands.value())).value();
    visit(overloaded{[](const double &val) { cout << val << endl; },
                     [](const int32_t &val) { cout << val << endl; },
                     [](const string &val) { cout << unquote(val) << endl; }},
          operand);
}

optional<variant<int32_t, double, string>> interpret(nodeType &p) {
    return visit(
        overloaded{
            [](constantNode &conNode)
                -> optional<variant<int32_t, double, string>> {
                return conNode.innerValue;
            },
            [](operatorNode &oprNode)
                -> optional<variant<int32_t, double, string>> {
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
            [](symbolNode &symNode)
                -> optional<variant<int32_t, double, string>> {
                if (symbols.find(symNode.symbol) == symbols.end()) {
                    cerr << "Undefined variable: " << symNode.symbol << endl;
                    abort();
                    return {};
                }
                const auto &sym = symbols[symNode.symbol];
                return sym.value;
            },
            [](vector<unique_ptr<nodeType>> &nodes)
                -> optional<variant<int32_t, double, string>> {
                for (const auto &node : nodes) {
                    interpret(*node);
                }
                return {};
            },
            [](auto &rest) -> optional<variant<int32_t, double, string>> {
                abort();
                return {};
            }},
        p.innerNode);
}
