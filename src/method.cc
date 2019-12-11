#include <cmath>

#include "method.h"

unordered_multimap<string, method> methodMap = {
    {"abs",
     {"abs",
      false,
      false,
      "int32",
      "System.Private.CoreLib",
      "System.Math",
      "Abs",
      {"int32"},
      (void *)&callerHelper<int32_t, static_cast<int32_t (*)(int32_t)>(
                                         &abs)>::inputOutput}},
    {"abs",
     {"abs",
      false,
      false,
      "float64",
      "System.Private.CoreLib",
      "System.Math",
      "Abs",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &abs)>::inputOutput}},
};

variant<int32_t, double, string>
method::call(vector<variant<int32_t, double, string>> params) const {
    return ((variant<int32_t, double, string>(*)(
        vector<variant<int32_t, double, string>>))(this->nativeImpl))(params);
}
