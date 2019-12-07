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

void exAssign(const operatorNode &opr) {
    const auto &[id_ptr, expr_ptr] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    const auto &id = *id_ptr;
    const auto &expr = *expr_ptr;
    const auto &variableNode = get<symbolNode>(id.innerNode);
    ex(expr);
    if (symbols.find(variableNode.symbol) == symbols.end()) {
        // Declare a new variable
        symbol sym;
        sym.literal = variableNode.symbol;
        sym.ilid = symbols.size();
        sym.type = "int32"; // TODO: type inference
        symbols[variableNode.symbol] = sym;
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
    const auto &[opr1, opr2] =
        get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
            opr.operands.value());
    ex(*opr1);
    ex(*opr2);
    ilbuf << '\t' << tokenToOperator.at(opr.operatorToken) << endl;
    // Negate negative operators
    switch (opr.operatorToken) {
    case token::GE:
    case token::LE:
    case token::NE:
        ilbuf << "\tldc.i4.0" << endl;
        ilbuf << "\tceq" << endl;
        maxStack = max(maxStack, currentStack + 1);
        break;
    }
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
            [](const unique_ptr<nodeType> &_body) {
                cerr << "if statement has no operands" << endl;
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

void ex(const nodeType &p) {
    visit(
        overloaded{
            [](const constantNode &conNode) {
                visit(overloaded{[](int value) {
                                     ilbuf << "\tldc.i4 " << value << endl;
                                     currentStack++;
                                     maxStack = max(maxStack, currentStack);
                                 },
                                 [](double _value) {
                                     cerr << "Double is not implemented"
                                          << endl;
                                     abort();
                                     // TODO: double
                                 }},
                      conNode.innerValue);
            },
            [](const operatorNode &oprNode) {
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
                    ex(*get<unique_ptr<nodeType>>(oprNode.operands.value()));
                    ilbuf << "\tcall void "
                             "[System.Console]System.Console::"
                             "WriteLine(int32)"
                          << endl;
                    currentStack--;
                    // TODO: type checking
                    break;
                default:
                    // Deal with binary operators
                    exBin(oprNode);
                    break;
                }
            },
            [](const symbolNode &symNode) {
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
            [](const vector<unique_ptr<nodeType>> &nodes) {
                for (const auto &node : nodes) {
                    ex(*node);
                }
            },
            [](auto &rest) { abort(); }},
        p.innerNode);
}
