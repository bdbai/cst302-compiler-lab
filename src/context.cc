#include "context.h"

ExpectedType Context::typeStringToExpected(const string &type) {
    if (type == "int32") {
        return ExpectedType::Integer;
    } else if (type == "float64") {
        return ExpectedType::Decimal;
    } else if (type == "string") {
        return ExpectedType::String;
    } else {
        cerr << "Unknown type to expect: " << type << endl;
        abort();
    }
}