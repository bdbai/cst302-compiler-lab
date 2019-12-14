#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <fstream>

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
      "mscorlib",
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
      "mscorlib",
      "System.Math",
      "Abs",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &abs)>::inputOutput}},
    {"acos",
     {"acos",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Acos",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &acos)>::inputOutput}},
    {"asin",
     {"asin",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Asin",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &asin)>::inputOutput}},
    {"atan",
     {"atan",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Atan",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &atan)>::inputOutput}},
    {"ceiling",
     {"ceiling",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Ceiling",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &ceil)>::inputOutput}},
    {"cos",
     {"cos",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Cos",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &cos)>::inputOutput}},
    {"cosh",
     {"cosh",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Cosh",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &cosh)>::inputOutput}},
    {"exp",
     {"exp",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Exp",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &exp)>::inputOutput}},
    {"floor",
     {"floor",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Floor",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &floor)>::inputOutput}},
    {"log",
     {"log",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Log",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &log)>::inputOutput}},
    {"log10",
     {"log10",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Log10",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &log10)>::inputOutput}},
    {"max",
     {"max",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Max",
      {"float64", "float64"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return max(get<double>(input[0]), get<double>(input[1]));
          })}},
    {"max",
     {"max",
      false,
      false,
      "int32",
      "mscorlib",
      "System.Math",
      "Max",
      {"int32", "int32"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return max(get<int32_t>(input[0]), get<int32_t>(input[1]));
          })}},
    {"min",
     {"min",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Min",
      {"float64", "float64"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return min(get<double>(input[0]), get<double>(input[1]));
          })}},
    {"min",
     {"min",
      false,
      false,
      "int32",
      "mscorlib",
      "System.Math",
      "Min",
      {"int32", "int32"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return min(get<int32_t>(input[0]), get<int32_t>(input[1]));
          })}},
    {"pow",
     {"pow",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Pow",
      {"float64", "float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double, double)>(
                                        &pow)>::inputOutput2}},
    {"round",
     {"round",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Round",
      {"float64"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return round(get<double>(input[0]));
          })}},
    {"round",
     {"round",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Round",
      {"float64", "int32"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              auto p = powl(10, get<int32_t>(input[1]));
              return (double)(round(p * get<double>(input[0])) / p);
          })}},
    {"sin",
     {"sin",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Sin",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &sin)>::inputOutput}},
    {"sinh",
     {"sinh",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Sinh",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &sinh)>::inputOutput}},
    {"sqrt",
     {"sqrt",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Sqrt",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &sqrt)>::inputOutput}},
    {"tan",
     {"tan",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Tan",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &tan)>::inputOutput}},
    {"tanh",
     {"tanh",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Tanh",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &tanh)>::inputOutput}},
    {"truncate",
     {"truncate",
      false,
      false,
      "float64",
      "mscorlib",
      "System.Math",
      "Truncate",
      {"float64"},
      (void *)&callerHelper<double, static_cast<double (*)(double)>(
                                        &trunc)>::inputOutput}},
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
      "mscorlib",
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
      "mscorlib",
      "System.Int32",
      "Parse",
      {"string"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return stoi(unquote(get<string>(input[0])));
          })}},
    {"toString",
     {"toString",
      true,
      false,
      "string",
      "mscorlib",
      "System.Int32",
      "ToString",
      {"int32"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return quote(to_string(get<int32_t>(input[0])));
          })}},
    {"toString",
     {"toString",
      true,
      false,
      "string",
      "mscorlib",
      "System.Double",
      "ToString",
      {"float64"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              return quote(to_string(get<double>(input[0])));
          })}},
    {"Random",
     {"Random",
      true,
      false,
      "void",
      "mscorlib",
      "System.Random",
      ".ctor",
      {},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>>)
              -> variant<int32_t, double, string> {
              return "System.Random";
          })}},
    {"randomDouble",
     {"randomDouble",
      true,
      true,
      "float64",
      "mscorlib",
      "System.Random",
      "NextDouble",
      {"class [mscorlib]System.Random"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>>)
              -> variant<int32_t, double, string> {
              static std::random_device rd;
              static std::mt19937 gen(rd());
              static std::uniform_real_distribution<> dis(0.0, 1.0);
              return dis(gen);
          })}},
    {"randomInt",
     {"randomInt",
      true,
      true,
      "int32",
      "mscorlib",
      "System.Random",
      "Next",
      {"class [mscorlib]System.Random"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>>)
              -> variant<int32_t, double, string> { return rand(); })}},
    {"randomInt",
     {"randomInt",
      true,
      true,
      "int32",
      "mscorlib",
      "System.Random",
      "Next",
      {"class [mscorlib]System.Random", "int32"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              static std::random_device rd;
              static std::mt19937 gen(rd());
              static std::uniform_real_distribution<> dis(
                  0.0, double(get<int32_t>(input[1])));
              return int32_t(dis(gen));
          })}},
    {"randomInt",
     {"randomInt",
      true,
      true,
      "int32",
      "mscorlib",
      "System.Random",
      "Next",
      {"class [mscorlib]System.Random", "int32", "int32"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              static std::random_device rd;
              static std::mt19937 gen(rd());
              static std::uniform_real_distribution<> dis(
                  double(get<int32_t>(input[1])),
                  double(get<int32_t>(input[2])));
              return int32_t(dis(gen));
          })}},
    {"fileExists",
     {"fileExists",
      false,
      false,
      "bool",
      "System.IO.FileSystem",
      "System.IO.File",
      "Exists",
      {"string"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              ifstream fs(unquote(get<string>(input[0])));
              return fs.good();
          })}},
    {"fileRead",
     {"fileRead",
      false,
      false,
      "string",
      "System.IO.FileSystem",
      "System.IO.File",
      "ReadAllText",
      {"string"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              std::ifstream fs(unquote(get<string>(input[0])));
              return quote(string((std::istreambuf_iterator<char>(fs)),
                                  std::istreambuf_iterator<char>()));
          })}},
    {"fileWrite",
     {"fileWrite",
      false,
      false,
      "void",
      "System.IO.FileSystem",
      "System.IO.File",
      "WriteAllText",
      {"string", "string"},
      (void *)(variant<int32_t, double, string> (*)(
          vector<variant<int32_t, double, string>>))(
          [](vector<variant<int32_t, double, string>> input)
              -> variant<int32_t, double, string> {
              fstream fs;
              fs.open(unquote(get<string>(input[0])),
                      fstream::out | fstream::trunc);
              fs << unquote(get<string>(input[1]));
              fs.flush();
              return 1;
          })}},
};

variant<int32_t, double, string>
method::call(vector<variant<int32_t, double, string>> params) const {
    return ((variant<int32_t, double, string>(*)(
        vector<variant<int32_t, double, string>>))(
        get<void *>(this->nativeImpl)))(params);
}
string method::getFullQualifier() const {
    stringstream buf;
    if (this->isInstance) {
        buf << "instance ";
    }
    buf << this->returnType << ' ';
    if (this->assemblyName != "calculator") {
        buf << this->getFullTypeQualifier() << ':' << ':';
    }
    buf << this->methodName << '(';
    for (size_t i = this->isInstance; i < this->parameters.size(); i++) {
        if (i != 0 && (i != 1 || !this->isInstance)) {
            buf << ',' << ' ';
        }
        buf << this->parameters[i];
    }
    buf << ')' << endl;
    return buf.str();
}
string method::getFullTypeQualifier() const {
    stringstream buf;
    if (this->assemblyName != "calculator") {
        buf << '[' << this->assemblyName << ']' << this->typeQualifier;
    }
    return buf.str();
}
optional<reference_wrapper<func>> method::getAsFunc() const {
    if (holds_alternative<shared_ptr<func>>(this->nativeImpl)) {
        return *get<shared_ptr<func>>(this->nativeImpl);
    } else {
        return {};
    }
}
