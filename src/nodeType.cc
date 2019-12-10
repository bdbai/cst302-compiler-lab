#include "nodeType.h"

map<string, symbol> symbols;

bool symbolNode::operator==(const symbolNode &other) const {
    return this->symbol == other.symbol;
}
bool symbolNode::operator<(const symbolNode &other) const {
    return this->symbol < other.symbol;
}

unique_ptr<nodeType> nodeType::make_constant(const int con) {
    return make_unique<nodeType>(constantNode(con), "int32");
}
unique_ptr<nodeType> nodeType::make_constant(const double con) {
    return make_unique<nodeType>(constantNode(con), "float64");
}
unique_ptr<nodeType> nodeType::make_constant(const string &con) {
    return make_unique<nodeType>(constantNode(con), "string");
}
unique_ptr<nodeType> nodeType::make_symbol(string sym) {
    return make_unique<nodeType>(symbolNode(sym));
}
unique_ptr<nodeType> nodeType::make_op(int opr) {
    operatorNode oprNode;
    oprNode.operatorToken = opr;
    return make_unique<nodeType>(move(oprNode));
}
unique_ptr<nodeType> nodeType::make_op(int opr, unique_ptr<nodeType> arg1) {
    operatorNode oprNode;
    oprNode.operatorToken = opr;
    oprNode.operands = move(arg1);
    return make_unique<nodeType>(move(oprNode));
}
unique_ptr<nodeType> nodeType::make_op(int opr, unique_ptr<nodeType> arg1,
                                       unique_ptr<nodeType> arg2) {
    operatorNode oprNode;
    oprNode.operatorToken = opr;
    oprNode.operands = pair<unique_ptr<nodeType>, unique_ptr<nodeType>>(
        move(arg1), move(arg2));
    return make_unique<nodeType>(move(oprNode));
}
unique_ptr<nodeType> nodeType::make_op(int opr, unique_ptr<nodeType> arg1,
                                       unique_ptr<nodeType> arg2,
                                       unique_ptr<nodeType> arg3) {
    operatorNode oprNode;
    oprNode.operatorToken = opr;
    oprNode.operands =
        tuple<unique_ptr<nodeType>, unique_ptr<nodeType>, unique_ptr<nodeType>>(
            move(arg1), move(arg2), move(arg3));
    return make_unique<nodeType>(move(oprNode));
}
unique_ptr<nodeType> nodeType::make_ops() {
    return make_unique<nodeType>(vector<unique_ptr<nodeType>>());
}
unique_ptr<nodeType> nodeType::make_ops(unique_ptr<nodeType> op) {
    vector<unique_ptr<nodeType>> nodes;
    nodes.push_back(move(op));
    return make_unique<nodeType>(move(nodes));
}
unique_ptr<nodeType> nodeType::make_opas(int opr, const string symbol,
                                         unique_ptr<nodeType> arg) {
    return move(
        nodeType::make_op('=', move(nodeType::make_symbol(symbol)),
                          move(nodeType::make_op(
                              opr, nodeType::make_symbol(symbol), move(arg)))));
}
void nodeType::push_op(unique_ptr<nodeType> op) {
    get<vector<unique_ptr<nodeType>>>(this->innerNode).push_back(move(op));
}
void nodeType::setType(const string &type) { this->type = type; }
const string getTypeCommon(const string &type1, const string &type2) {
    if (type1 == type2) {
        return type1;
    }
    if ((type1 == "int32" && type2 == "float64") ||
        (type1 == "float64" && type2 == "int32")) {
        return "float64";
    }
    cerr << "Cannot get common type of " << type1 << " and " << type2 << endl;
    abort();
}
const string nodeType::inferType() {
    if (this->type.has_value()) {
        return this->type.value();
    }
    const auto result = visit(
        overloaded{
            [](const constantNode &) -> const string {
                cerr << "Cannot infer type of constant node" << endl;
                abort();
            },
            [](operatorNode &oprNode) -> const string {
                if (!oprNode.operands.has_value()) {
                    return "!";
                }
                return visit(
                    overloaded{
                        [](unique_ptr<nodeType> &opr) -> const string {
                            return opr->inferType();
                        },
                        [](pair<unique_ptr<nodeType>, unique_ptr<nodeType>>
                               &opr) -> const string {
                            return getTypeCommon(opr.first->inferType(),
                                                 opr.second->inferType());
                        },
                        [](tuple<unique_ptr<nodeType>, unique_ptr<nodeType>,
                                 unique_ptr<nodeType>> &opr) -> const string {
                            const auto &[opr1, opr2, opr3] = opr;
                            return getTypeCommon(
                                getTypeCommon(opr1->inferType(),
                                              opr2->inferType()),
                                opr3->inferType());
                        },
                    },
                    oprNode.operands.value());
            },
            [](const symbolNode &symNode) -> const string {
                if (symbols.find(symNode.symbol) == symbols.end()) {
                    cerr << "Cannot infer type of non-existing symbol "
                         << symNode.symbol << endl;
                    abort();
                }
                return symbols.at(symNode.symbol).type;
            },
            [](const vector<unique_ptr<nodeType>> &) -> const string {
                return "!";
            }},
        this->innerNode);
    this->type = result;
    return result;
}