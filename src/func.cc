#include "func.h"

multimap<string, shared_ptr<func>> func::definedFunctions;

symbol func::make_param(string type, string literal) {
    symbol sym;
    sym.ilpostfix = "arg";
    sym.literal = literal;
    sym.type = type;
    return sym;
}

shared_ptr<func> func::make_func(string name, vector<symbol> params,
                                 string returnType,
                                 unique_ptr<nodeType> bodyStmts) {
    unordered_set<string> param_names;
    for (const auto &sym : params) {
        const auto inserted = param_names.insert(sym.literal).second;
        if (!inserted) {
            cerr << "Duplicated parameter name: " << sym.literal << endl;
            abort();
        }
    }
    if (!holds_alternative<vector<unique_ptr<nodeType>>>(
            bodyStmts->innerNode)) {
        cerr << "Function body can only be statement block" << endl;
        abort();
    }
    vector<string> param_types;
    for (const auto &param : params) {
        param_types.push_back(param.type);
    }
    const auto ret = make_shared<func>(
        name, params, returnType,
        move(get<vector<unique_ptr<nodeType>>>(bodyStmts->innerNode)));
    methodMap.insert(
        make_pair(name, method(name, false, false, returnType, "calculator", "",
                               name, param_types, ret)));
    return ret;
}