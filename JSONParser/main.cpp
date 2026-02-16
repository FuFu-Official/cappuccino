#include "print.hpp"
#include <charconv>
#include <regex>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <variant>
#include <vector>

struct JSONObject {
  std::variant<std::monostate,                             // none
               bool,                                       // true & false
               int,                                        // 3
               double,                                     // 3,14
               std::string,                                // "hello"
               std::vector<JSONObject>,                    // [true, 3]
               std::unordered_map<std::string, JSONObject> // {"hello": 3}
               >
      inner;

  void do_print() const { print(inner); }
};

JSONObject parse(std::string_view json);

char unescaped_char(char c);

template <class T> std::optional<T> try_parse_num(std::string_view str);

int main() {
  std::string_view str = R"JSON("\n")JSON";
  print(parse(str));
  return 0;
}

JSONObject parse(std::string_view json) {
  // Parse none
  if (json.empty()) {
    return JSONObject{std::monostate{}};
  }

  // Parse int & double
  if (char ch = json[0]; (ch >= '0' && ch <= '9') || ch == '+' || ch == '-') {
    std::regex num_regex{"-?(0|[1-9][0-9]*)(\\.[0-9]+)?([eE][+-]?[0-9]+)?"};
    std::cmatch match;
    if (std::regex_search(json.data(), json.data() + json.size(), match,
                          num_regex)) {
      std::string str = match.str();
      if (auto num = try_parse_num<int>(str); num.has_value()) {
        return JSONObject{num.value()};
      }

      if (auto num = try_parse_num<double>(str); num.has_value()) {
        return JSONObject{num.value()};
      }
    }
  }

  // Parse string
  if (json[0] == '"') {
    std::string str;
    enum { Raw, Esc } phase = Raw;

    size_t i = 1;
    for (; i < json.size(); ++i) {
      char ch = json[i];
      if (phase == Raw) {
        if (ch == '\\') {
          phase = Esc;
          continue;
        } else if (ch == '"') {
          i++;
          break;
        } else {
          str += ch;
        }
      }

      if (phase == Esc) {
        str += unescaped_char(ch);
        phase = Raw;
      }
    }

    return JSONObject{std::move(str)};
  }

  // TODO: Parse List

  return JSONObject{std::monostate{}};
}

template <class T> std::optional<T> try_parse_num(std::string_view str) {
  T value;
  auto res = std::from_chars(str.data(), str.data() + str.size(), value);
  if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
    return value;
  }
  return std::nullopt;
}

char unescaped_char(char c) {
  switch (c) {
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case '0':
    return '\0';
  case 't':
    return '\t';
  case 'v':
    return '\v';
  case 'f':
    return '\f';
  case 'b':
    return '\b';
  case 'a':
    return '\a';
  default:
    return c;
  }
}
