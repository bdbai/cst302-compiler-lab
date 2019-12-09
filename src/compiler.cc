#include "compiler.h"

void before_processing() {
    cout << ".assembly calculator {} " << endl
         << ".method public hidebysig static void Main () cil managed " << endl
         << "{" << endl;
}

void after_processing() {
    if (symbols.size() > 0) {
        // Print locals
        cout << "\t.locals init (" << endl;
        bool first = true;
        for (const auto &[_, sym] : symbols) {
            if (first) {
                first = false;
            } else {
                cout << ',' << endl;
            }
            cout << "\t\t[" << sym.ilid << "] " << sym.type;
        }
        cout << endl << "\t)" << endl;
    }
    // Print buffered CIL code
    cout << "\t.maxstack " << maxStack << endl
         << "\t.entrypoint" << endl
         << ilbuf.str();
    // Print tail
    cout << "\tret" << endl << "}" << endl;
}

void convertType(const string &typeFrom, const string &typeTo) {
    if (typeFrom == typeTo) {
        return;
    }
    if (typeFrom == "int32" && typeTo == "float64") {
        ilbuf << "\tconv.r8" << endl;
    } else if (typeFrom == "float64" && typeTo == "int32") {
        ilbuf << "\tconv.i4" << endl;
    } else {
        cerr << "Cannot convert " << typeFrom << " to " << typeTo << endl;
        abort();
    }
}
void convertType(const string &typeFrom, const ExpectedType expecting) {
    switch (expecting) {
    case ExpectedType::Decimal:
        convertType(typeFrom, "float64");
        break;
    case ExpectedType::Integer:
        convertType(typeFrom, "int32");
        break;
    case ExpectedType::String:
        convertType(typeFrom, "string");
        break;
    case ExpectedType::None:
        // Do nothing
        break;
    }
}

void exAssign(const operatorNode &opr) {
    const auto &[id_ptr, expr_ptr] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    const auto &id = *id_ptr;
    auto &expr = *expr_ptr;
    const auto &variableNode = get<symbolNode>(id.innerNode);
    const auto &type = expr.inferType();
    auto ctx = Context();
    ctx.expecting = Context::typeStringToExpected(type);
    ex(expr, ctx);
    const auto symbolIt = symbols.find(variableNode.symbol);
    if (symbolIt == symbols.end()) {
        // Declare a new variable
        symbol sym;
        sym.literal = variableNode.symbol;
        sym.ilid = symbols.size();
        sym.type = type;
        symbols[variableNode.symbol] = sym;
    } else {
        convertType(type, symbolIt->second.type);
    }
    const auto &sym = symbols[variableNode.symbol];
    ilbuf << "\tstloc " << sym.ilid << endl;
    currentStack--;
}

void exBin(const operatorNode &opr) {
    if (tokenToOperator.find(opr.operatorToken) == tokenToOperator.end()) {
        cerr << "Cannot find opr" << endl;
        abort();
    }
    auto &[opr1, opr2] = get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
        opr.operands.value());
    const auto &type1 = opr1->inferType();
    const auto &type2 = opr2->inferType();
    auto commonType = getTypeCommon(type1, type2);
    if (opr.operatorToken == token::SHL || opr.operatorToken == token::SHR) {
        commonType = "int32";
    }
    auto ctx = Context();
    ctx.expecting = Context::typeStringToExpected(commonType);
    ex(*opr1, ctx);
    ex(*opr2, ctx);
    if (commonType == "int32" || commonType == "float64") {
        ilbuf << '\t' << tokenToOperator.at(opr.operatorToken) << endl;
        // Negate negative operators
        switch (opr.operatorToken) {
        case token::GE:
        case token::LE:
        case token::NE:
            ilbuf << "\tldc.i4.0" << endl;
            ilbuf << "\tceq" << endl;
            break;
        }
    } else if (commonType == "string" && opr.operatorToken == '+') {
        ilbuf << "\tcall string "
                 "[System.Private.CoreLib]System.String::Concat(string, string)"
              << endl;
    } else {
        cerr << "Cannot perform binary operation " << opr.operatorToken
             << " on " << type1 << " and " << type2 << endl;
        abort();
    }
    currentStack -= 1;
}

void exWhile(const operatorNode &p) {
    const auto checkLabel = ++label, loopLabel = ++label;
    const auto &[condNode, bodyNode] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            p.operands.value());
    ilbuf << "\tbr.s LABEL" << checkLabel << endl;
    ilbuf << "\tLABEL" << loopLabel << ": ";
    ex(*bodyNode);
    ilbuf << "\tLABEL" << checkLabel << ": ";
    ex(*condNode);
    ilbuf << "\tbrtrue.s LABEL" << loopLabel << endl;
    currentStack--;
}

void exIf(const operatorNode &p) {
    visit(
        overloaded{
            [](const unique_ptr<nodeType> &) {
                cerr << "if statement has only 1 operand" << endl;
                abort();
            },
            [](const pair<unique_ptr<nodeType>, unique_ptr<nodeType>> &ifBody) {
                const auto &[condNodePtr, bodyNodePtr] = ifBody;
                const auto falseLabel = ++label;
                ex(*condNodePtr);
                ilbuf << "\tbrfalse.s LABEL" << falseLabel << endl;
                currentStack--;
                ex(*bodyNodePtr);
                ilbuf << "\tLABEL" << falseLabel << ": ";
            },
            [](const tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                           unique_ptr<nodeType>> &ifElseBody) {
                const auto &[condNodePtr, bodyNodePtr, elseNodePtr] =
                    ifElseBody;
                const auto falseLabel = ++label, nextLabel = ++label;
                ex(*condNodePtr);
                ilbuf << "\tbrfalse.s LABEL" << falseLabel << endl;
                currentStack--;
                ex(*bodyNodePtr);
                ilbuf << "\tbr.s LABEL" << nextLabel << endl;
                ilbuf << "\tLABEL" << falseLabel << ": ";
                ex(*elseNodePtr);
                ilbuf << "\tLABEL" << nextLabel << ": ";
            }},
        p.operands.value());
}
void exPrint(const operatorNode &p) {
    auto &oprNode = *get<unique_ptr<nodeType>>(p.operands.value());
    const auto &type = oprNode.inferType();
    ex(oprNode);
    ilbuf << "\tcall void "
             "[System.Console]System.Console::"
             "WriteLine("
          << type << ")" << endl;
    currentStack--;
}

void ex(nodeType &p) {
    ex(p, {});
    // TODO: stack balancing
}

void ex(nodeType &p, Context ctx) {
    visit(
        overloaded{
            [](constantNode &conNode) {
                visit(overloaded{[](const int value) {
                                     ilbuf << "\tldc.i4 " << value << endl;
                                 },
                                 [](const double value) {
                                     ilbuf << "\tldc.r8 " << value << endl;
                                 },
                                 [](const string &value) {
                                     ilbuf << "\tldstr " << value << endl;
                                 }},
                      conNode.innerValue);
                currentStack++;
                maxStack = max(maxStack, currentStack);
            },
            [](operatorNode &oprNode) {
                switch (oprNode.operatorToken) {
                case token::WHILE:
                    exWhile(oprNode);
                    break;
                case token::IF:
                    exIf(oprNode);
                    break;
                case '=':
                    exAssign(oprNode);
                    break;
                case token::PRINT:
                    exPrint(oprNode);
                    break;
                case token::UMINUS:
                    ex(*get<unique_ptr<nodeType>>(oprNode.operands.value()));
                    ilbuf << "\tneg" << endl;
                    break;
                default:
                    // Deal with binary operators
                    exBin(oprNode);
                    break;
                }
            },
            [](symbolNode &symNode) {
                if (symbols.find(symNode.symbol) == symbols.end()) {
                    cerr << "Undefined variable: " << symNode.symbol << endl;
                    abort();
                    return;
                }
                const auto &sym = symbols[symNode.symbol];
                ilbuf << "\tldloc " << sym.ilid << endl;
                currentStack++;
                maxStack = max(maxStack, currentStack);
            },
            [](vector<unique_ptr<nodeType>> &nodes) {
                for (const auto &node : nodes) {
                    ex(*node);
                }
            },
            [](auto &) { abort(); }},
        p.innerNode);
    if (ctx.expecting != ExpectedType::None) {
        convertType(p.inferType(), ctx.expecting);
    }
}
