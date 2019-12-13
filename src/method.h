#ifndef METHOD_H
#define METHOD_H
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
class func;

using namespace std;

template <typename T, auto fn> class callerHelper {
  public:
    static variant<int32_t, double, string>
    inputOutput(vector<variant<int32_t, double, string>> params) {
        return ((T(*)(T))(fn))(get<T>(params[0]));
    }
};

class method {
  private:
    variant<void *, shared_ptr<func>> nativeImpl;

  public:
    method(const string alias, const bool isInstance, const bool isVirtual,
           const string returnType, const string assemblyName,
           const string typeQualifier, const string methodName,
           const vector<string> parameters,
           variant<void *, shared_ptr<func>> nativeImpl)
        : nativeImpl(nativeImpl), alias(alias), isInstance(isInstance),
          isVirtual(isVirtual), returnType(returnType),
          assemblyName(assemblyName), typeQualifier(typeQualifier),
          methodName(methodName), parameters(parameters) {}
    string alias;
    bool isInstance;
    bool isVirtual;
    string returnType;
    string assemblyName;
    string typeQualifier;
    string methodName;
    vector<string> parameters;
    variant<int32_t, double, string>
    call(vector<variant<int32_t, double, string>>) const;
    optional<reference_wrapper<func>> getAsFunc() const;
};

string string_replace(const string &haystack, const string &needle,
                      const std::string &replaceStr);
string quote(const string &s);
string unquote(const string &s);

extern unordered_multimap<string, method> methodMap;

#endif
