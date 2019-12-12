#include <cmath>
#include <iostream>
#include <limits>

#include "method.h"
void string_replace(string &haystack, const string &needle,
                    const std::string &replaceStr) {
    if (needle.empty()) {
        return;
    }
    size_t pos;
    while ((pos = haystack.find(needle)) != string::npos) {
        haystack.replace(pos, needle.length(), replaceStr);
        pos += replaceStr.length();
    }
}
string quote(const string &s) {
    static vector<pair<string, string>> patterns = {{"\\", "\\\\"},
                                                    {
                                                        "\n",
                                                        "\\n",
                                                    },
                                                    {
                                                        "\r",
                                                        "\\r",
                                                    },
                                                    {
                                                        "\t",
                                                        "\\t",
                                                    },
                                                    {"\"", "\\\""}};
    string result = s;
    for (const auto &p : patterns) {
        string_replace(result, p.first, p.second);
    }
    // Add surrounding quotes
    result = "\"" + result + "\"";
    return result;
}
string unquote(const string &s) {
    static vector<pair<string, string>> patterns = {{"\\\\", "\\"},
                                                    {"\\n", "\n"},
                                                    {"\\r", "\r"},
                                                    {"\\t", "\t"},
                                                    {"\\\"", "\""}};
    string result = s;
    if (result.length() >= 2) {
        // Remove surrounding quotes
        result = result.substr(1, result.length() - 2);
    }
    for (const auto &p : patterns) {
        string_replace(result, p.first, p.second);
    }
    return result;
}

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
    {"input",
     {"input",
      false,
      false,
      "string",
      "System.Console",
      "System.Console",
      "ReadLine",
      {},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>>)
              -> variant<int32_t, double, string> {
              string str;
              // Avoid newline from source code as input
              cin.ignore(numeric_limits<streamsize>::max(), '\n');
              getline(cin, str);
              return quote(str);
          })}},
    {"parseDouble",
     {"parseDouble",
      false,
      false,
      "float64",
      "System.Private.CoreLib",
      "System.Double",
      "Parse",
      {"string"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return stod(unquote(get<string>(input[0])));
          })}},
    {"parseInt",
     {"parseInt",
      false,
      false,
      "int32",
      "System.Private.CoreLib",
      "System.Int32",
      "Parse",
      {"string"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return stoi(unquote(get<string>(input[0])));
          })}},
};

variant<int32_t, double, string>
method::call(vector<variant<int32_t, double, string>> params) const {
    return ((variant<int32_t, double, string>(*)(
        vector<variant<int32_t, double, string>>))(this->nativeImpl))(params);
}
