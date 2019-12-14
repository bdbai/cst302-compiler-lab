#include "compiler.h"

auto fn = func::make_func("Main", {}, "void", nodeType::make_ops());
Context ctx(fn);

void before_processing() {
    Lexer::isCompiler = true;
    globalOut << ".assembly calculator {} " << endl;
    beginFunction(globalOut);
}

void beginFunction(ostream &out) {
    out << ".method public hidebysig static " << ctx.currentFunc->returnType
        << " " << ctx.currentFunc->name << " (";
    bool first = true;
    for (const auto &param : ctx.currentFunc->params) {
        if (first) {
            first = false;
            out << '\n';
        }
        out << '\t' << param.type << endl;
    }
    out << ") cil managed " << endl << "{" << endl;
}

void after_processing() {
    endFunction(globalOut);
    for (const auto &funcIl : funcIls) {
        globalOut << funcIl;
    }
    cout << globalOut.str();
}

void endFunction(ostream &out) {
    if (symbols.size() > 0) {
        // Print locals
        out << "\t.locals init (" << endl;
        bool first = true;
        for (const auto &[_, sym] : symbols) {
            if (sym.ilpostfix != "loc") {
                continue;
            }
            if (first) {
                first = false;
            } else {
                out << ',' << endl;
            }
            out << "\t\t[" << sym.ilid << "] " << sym.type;
        }
        out << endl << "\t)" << endl;
    }
    // Print buffered CIL code
    out << "\t.maxstack " << ctx.maxStack << endl;
    if (ctx.currentFunc->name == "Main") {
        out << "\t.entrypoint" << endl;
    }
    out << ctx.ilbuf.str();
    out << "\tret" << endl;
    // Print tail
    out << "}" << endl;
}

void convertType(const string &typeFrom, const string &typeTo) {
    if (typeFrom == typeTo) {
        return;
    }
    if (typeFrom == "int32" && typeTo == "float64") {
        ctx.ilbuf << "\tconv.r8" << endl;
    } else if (typeFrom == "float64" && typeTo == "int32") {
        ctx.ilbuf << "\tconv.i4" << endl;
    } else {
        cerr << "Cannot convert " << typeFrom << " to " << typeTo << endl;
        abort();
    }
}
void convertType(const string &typeFrom, const ExpectedType expecting) {
    switch (expecting) {
    case ExpectedType::Any:
        break;
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
    ex(expr, Context::typeStringToExpected(type));
    const auto symbolIt = symbols.find(variableNode.symbol);
    if (symbolIt == symbols.end()) {
        // Declare a new variable
        symbol sym;
        sym.literal = variableNode.symbol;
        sym.ilid = ctx.currentLoc++;
        sym.ilpostfix = "loc";
        sym.type = type;
        symbols[variableNode.symbol] = sym;
    } else {
        convertType(type, symbolIt->second.type);
    }
    const auto &sym = symbols[variableNode.symbol];
    ctx.ilbuf << "\tst" << sym.ilpostfix << ' ' << sym.ilid << endl;
    ctx.currentStack--;
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
    if (commonType == "!") {
        cerr << "Cannot perform binary operation " << opr.operatorToken
             << " on " << type1 << " and " << type2 << endl;
        abort();
    }
    if (opr.operatorToken == token::SHL || opr.operatorToken == token::SHR) {
        commonType = "int32";
    }
    auto expecting = Context::typeStringToExpected(commonType);
    ex(*opr1, expecting);
    ex(*opr2, expecting);
    if (commonType == "int32" || commonType == "float64") {
        ctx.ilbuf << '\t' << tokenToOperator.at(opr.operatorToken) << endl;
        // Negate negative operators
        switch (opr.operatorToken) {
        case token::GE:
        case token::LE:
        case token::NE:
            ctx.ilbuf << "\tldc.i4.0" << endl;
            ctx.ilbuf << "\tceq" << endl;
            break;
        }
    } else if (commonType == "string" && opr.operatorToken == '+') {
        ctx.ilbuf
            << "\tcall string [mscorlib]System.String::Concat(string, string)"
            << endl;
    } else if (commonType == "string" && opr.operatorToken == token::EQ) {
        ctx.ilbuf << "\tcall bool [mscorlib]System.String::op_Equality(string, "
                     "string)"
                  << endl;
    } else if (commonType == "string" && opr.operatorToken == token::NE) {
        ctx.ilbuf
            << "\tcall bool [mscorlib]System.String::op_Inequality(string, "
               "string)"
            << endl;
    } else {
        cerr << "Cannot perform binary operation " << opr.operatorToken
             << " on " << type1 << " and " << type2 << endl;
        abort();
    }
    ctx.currentStack -= 1;
}

void exLoop(nodeType &condNode, nodeType &bodyNode,
            optional<reference_wrapper<nodeType>> nextNode) {
    const auto checkLabel = ++label, loopLabel = ++label;
    auto nextLabel = checkLabel;
    if (nextNode.has_value()) {
        nextLabel = ++label;
    }
    ctx.continueJump.push(nextLabel);
    ctx.breakJump.push({});
    ctx.ilbuf << "\tbr LABEL" << checkLabel << endl;
    ctx.ilbuf << "\tLABEL" << loopLabel << ": ";
    ex(bodyNode, ExpectedType::None);
    if (nextNode.has_value()) {
        ctx.ilbuf << "\tLABEL" << nextLabel << ": ";
        ex(nextNode.value());
    }
    ctx.ilbuf << "\tLABEL" << checkLabel << ": ";
    ex(condNode, ExpectedType::Any);
    ctx.ilbuf << "\tbrtrue LABEL" << loopLabel << endl;
    ctx.currentStack--;
    ctx.continueJump.pop();
    const auto breakLabel = ctx.breakJump.top();
    if (breakLabel.has_value()) {
        ctx.ilbuf << "\tLABEL" << breakLabel.value() << ": ";
    }
    ctx.breakJump.pop();
}

void exWhile(const operatorNode &p) {
    auto &[cond, stmts] = get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
        p.operands.value());
    exLoop(*cond, *stmts, {});
}

void exFor(const operatorNode &p) {
    auto &[init, whileNode, next] =
        get<tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                  unique_ptr<nodeType>>>(p.operands.value());
    ex(*init);
    auto &[cond, stmt] = get<pair<unique_ptr<nodeType>, unique_ptr<nodeType>>>(
        get<operatorNode>(whileNode->innerNode).operands.value());
    if (holds_alternative<vector<unique_ptr<nodeType>>>(cond->innerNode)) {
        exLoop(*nodeType::make_constant(1), *stmt, *next);
    } else {
        exLoop(*cond, *stmt, *next);
    }
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
                ex(*condNodePtr, ExpectedType::Any);
                ctx.ilbuf << "\tbrfalse LABEL" << falseLabel << endl;
                ctx.currentStack--;
                ex(*bodyNodePtr, ExpectedType::None);
                ctx.ilbuf << "\tLABEL" << falseLabel << ": ";
            },
            [](const tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                           unique_ptr<nodeType>> &ifElseBody) {
                const auto &[condNodePtr, bodyNodePtr, elseNodePtr] =
                    ifElseBody;
                const auto falseLabel = ++label, nextLabel = ++label;
                ex(*condNodePtr, ExpectedType::Any);
                ctx.ilbuf << "\tbrfalse LABEL" << falseLabel << endl;
                ctx.currentStack--;
                ex(*bodyNodePtr, ExpectedType::None);
                ctx.ilbuf << "\tbr LABEL" << nextLabel << endl;
                ctx.ilbuf << "\tLABEL" << falseLabel << ": ";
                ex(*elseNodePtr, ExpectedType::None);
                ctx.ilbuf << "\tLABEL" << nextLabel << ": ";
            }},
        p.operands.value());
}
void exPrint(const operatorNode &p) {
    auto &oprNode = *get<unique_ptr<nodeType>>(p.operands.value());
    const auto &type = oprNode.inferType();
    ex(oprNode, Context::typeStringToExpected(type));
    ctx.ilbuf << "\tcall void "
                 "[System.Console]System.Console::"
                 "WriteLine("
              << type << ")" << endl;
    ctx.currentStack--;
}
void exUminus(const operatorNode &p) {
    auto &opr = get<unique_ptr<nodeType>>(p.operands.value());
    ex(*opr, Context::typeStringToExpected(opr->inferType()));
    ctx.ilbuf << "\tneg" << endl;
}

void exContinue() {
    if (ctx.continueJump.empty()) {
        cerr << "continue statement is not allowed without an enclosing loop"
             << endl;
        abort();
    }
    ctx.ilbuf << "\tbr LABEL" << ctx.continueJump.top() << endl;
}
void exBreak() {
    if (ctx.breakJump.empty()) {
        cerr << "break statement is not allowed without an enclosing loop"
             << endl;
        abort();
    }
    auto &jumpTop = ctx.breakJump.top();
    if (!jumpTop.has_value()) {
        jumpTop.emplace(++label);
    }
    ctx.ilbuf << "\tbr LABEL" << jumpTop.value() << endl;
}

void ex(nodeType &p) {
    ex(p, {});
    while (ctx.currentStack > 0) {
        ctx.ilbuf << "\tpop" << endl;
        ctx.currentStack--;
    }
}

void ex(nodeType &p, ExpectedType expecting) {
    visit(
        overloaded{
            [](constantNode &conNode) {
                visit(overloaded{[](const int value) {
                                     ctx.ilbuf << "\tldc.i4 " << value << endl;
                                 },
                                 [](const double value) {
                                     ctx.ilbuf << "\tldc.r8 " << value << endl;
                                 },
                                 [](const string &value) {
                                     ctx.ilbuf << "\tldstr " << value << endl;
                                 }},
                      conNode.innerValue);
                ctx.currentStack++;
                ctx.maxStack = max(ctx.maxStack, ctx.currentStack);
            },
            [](operatorNode &oprNode) {
                switch (oprNode.operatorToken) {
                case token::FOR:
                    exFor(oprNode);
                    break;
                case token::WHILE:
                    exWhile(oprNode);
                    break;
                case token::CONTINUE:
                    exContinue();
                    break;
                case token::BREAK:
                    exBreak();
                    break;
                case token::RETURN:
                    if (ctx.currentFunc->returnType == "void") {
                        if (oprNode.operands.has_value()) {
                            cerr << "A function with return type \"void\" "
                                    "cannot return with any value"
                                 << endl;
                            abort();
                        }
                    } else {
                        if (!oprNode.operands.has_value()) {
                            cerr << "A function with non-void return type must "
                                    "return with a value"
                                 << endl;
                            abort();
                        }
                        ex(*get<unique_ptr<nodeType>>(oprNode.operands.value()),
                           Context::typeStringToExpected(
                               ctx.currentFunc->returnType));
                        ctx.currentStack--;
                    }
                    ctx.ilbuf << "\tret" << endl;
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
                    exUminus(oprNode);
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
                ctx.ilbuf << "\tld" << sym.ilpostfix << " " << sym.ilid << endl;
                ctx.currentStack++;
                ctx.maxStack = max(ctx.maxStack, ctx.currentStack);
            },
            [](vector<unique_ptr<nodeType>> &nodes) {
                if (nodes.size() == 0) {
                    ctx.ilbuf << "\tnop" << endl;
                } else {
                    for (const auto &node : nodes) {
                        ex(*node);
                    }
                }
            },
            [](callNode &cNode) {
                const auto &method = cNode.resolveMethod();
                if (method.has_value()) {
                    const auto &resolvedMethod = method.value().get();
                    for (size_t i = 0; i < resolvedMethod.parameters.size();
                         i++) {
                        ex(*cNode.params[i], Context::typeStringToExpected(
                                                 resolvedMethod.parameters[i]));
                        if (i == 0 && resolvedMethod.isInstance &&
                            (cNode.params[i]->inferType() == "int32" ||
                             cNode.params[i]->inferType() == "float64")) {
                            symbol sym;
                            sym.literal = "intermediate symbol";
                            sym.ilid = ctx.currentLoc++;
                            sym.ilpostfix = "loc";
                            sym.type = cNode.params[i]->inferType();
                            symbols[sym.literal + to_string(sym.ilid)] = sym;
                            ctx.ilbuf << "\tst" << sym.ilpostfix << ' '
                                      << sym.ilid << endl;
                            ctx.ilbuf << "\tld" << sym.ilpostfix << 'a' << ' '
                                      << sym.ilid << endl;
                        }
                    }

                    if (cNode.isCtor) {
                        ctx.ilbuf << "\tnewobj ";
                    } else if (resolvedMethod.isVirtual) {
                        ctx.ilbuf << "\tcallvirt ";
                    } else {
                        ctx.ilbuf << "\tcall ";
                    }
                    ctx.ilbuf << resolvedMethod.getFullQualifier();
                    ctx.currentStack -= resolvedMethod.parameters.size() - 1;
                } else {
                    cerr << "Cannot find an overloaded method: " << cNode.func
                         << endl;
                    abort();
                }
            }},
        p.innerNode);
    if (expecting != ExpectedType::None && expecting != ExpectedType::Any) {
        convertType(p.inferType(), expecting);
    }
}

void exFunc(shared_ptr<func> fn) {
    // Save old context and symbols
    auto globalCtx(move(ctx));
    auto globalSymbols(move(symbols));

    // Arrange symbols
    symbols = {};
    for (size_t i = 0; i < fn->params.size(); i++) {
        symbol sym(fn->params[i]);
        symbols[sym.literal] = sym;
    }

    // Prepare new context
    Context newCtx(fn);
    newCtx.ilbuf = stringstream();
    ctx = move(newCtx);

    // Walk through function statements
    stringstream newbuf;
    beginFunction(newbuf);
    for (auto &stmt : fn->bodyStmts) {
        ex(*stmt);
    }
    endFunction(newbuf);

    // Recover context
    symbols = move(globalSymbols);
    ctx = move(globalCtx);

    // Save IL
    funcIls.push_back(newbuf.str());
}
