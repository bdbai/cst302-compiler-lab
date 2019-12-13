#ifndef FUNC_H
#define FUNC_H

#include <unordered_set>
#include <map>

#include "nodeType.h"

class func {
  public:
    func(string name, vector<symbol> params, string returnType,
         vector<unique_ptr<nodeType>> bodyStmts)
        : name(name), params(params), returnType(returnType),
          bodyStmts(move(bodyStmts)) {}
    string name;
    vector<symbol> params;
    string returnType;
    vector<unique_ptr<nodeType>> bodyStmts;
    static symbol make_param(string type, string literal);
    static shared_ptr<func> make_func(string name, vector<symbol> params,
                                      string returnType,
                                      unique_ptr<nodeType> bodyStmts);
    static multimap<string, shared_ptr<func>> definedFunctions;
};

#endif
