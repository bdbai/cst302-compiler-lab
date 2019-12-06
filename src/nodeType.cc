#include "nodeType.h"

bool symbolNode::operator==(const symbolNode &other) const {
    return this->symbol == other.symbol;
}
bool symbolNode::operator<(const symbolNode &other) const {
    return this->symbol < other.symbol;
}

unique_ptr<nodeType> nodeType::make_constant(int con) {
    return make_unique<nodeType>(constantNode(con));
}
unique_ptr<nodeType> nodeType::make_constant(double con) {
    return make_unique<nodeType>(constantNode(con));
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
