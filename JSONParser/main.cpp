#include "print.hpp"
#include <charconv>
#include <cstddef>
#include <regex>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>
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

  void do_print() const { printnl(inner); }
};

char unescaped_char(char c);

template <class T> std::optional<T> try_parse_num(std::string_view str);

std::pair<JSONObject, size_t> parse(std::string_view json);

int main() {
  std::string_view str = R"JSON([1,[1,3,    "hellp"          ],5])JSON";
  print(parse(str).first);
  return 0;
}

std::pair<JSONObject, size_t> parse(std::string_view json) {
  // Parse none
  if (json.empty()) {
    return {JSONObject{std::monostate{}}, 0};
  }

  // Parse int & double
  if (char ch = json[0]; (ch >= '0' && ch <= '9') || ch == '+' || ch == '-') {
    std::regex num_regex{"-?(0|[1-9][0-9]*)(\\.[0-9]+)?([eE][+-]?[0-9]+)?"};
    std::cmatch match;
    if (std::regex_search(json.data(), json.data() + json.size(), match,
                          num_regex)) {
      std::string str = match.str();
      if (auto num = try_parse_num<int>(str); num.has_value()) {
        return {JSONObject{num.value()}, str.size()};
      }

      if (auto num = try_parse_num<double>(str); num.has_value()) {
        return {JSONObject{num.value()}, str.size()};
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

    return {JSONObject{std::move(str)}, i};
  }

  auto skip_whitespace = [&json](size_t &i) {
    while (i < json.size() &&
           std::isspace(static_cast<unsigned char>(json[i]))) {
      ++i;
    }
  };

  // Parse list
  if (json[0] == '[') {
    std::vector<JSONObject> res;
    size_t i;
    for (i = 1; i < json.size();) {
      if (json[i] == ']') {
        i += 1;
        break;
      }
      auto [obj, eaten] = parse(json.substr(i));
      if (eaten == 0) {
        i = 0;
        break;
      }
      res.push_back(std::move(obj));
      i += eaten;

      skip_whitespace(i);
      if (json[i] == ',') {
        i += 1;
      }
      skip_whitespace(i);
    }
    return {JSONObject{std::move(res)}, i};
  }

  // Parse dict
  if (json[0] == '{') {
    std::unordered_map<std::string, JSONObject> res;
    size_t i;
    for (i = 1; i < json.size();) {
      if (json[i] == '}') {
        i += 1;
        break;
      }
      auto [keyobj, keyeaten] = parse(json.substr(i));
      if (keyeaten == 0) {
        i = 0;
        break;
      }
      i += keyeaten;
      if (!std::holds_alternative<std::string>(keyobj.inner)) {
        i = 0;
        break;
      }
      if (json[i] == ':') {
        i += 1;
      }
      std::string key = std::move(std::get<std::string>(keyobj.inner));
      auto [valobj, valeaten] = parse(json.substr(i));
      if (valeaten == 0) {
        i = 0;
        break;
      }
      i += valeaten;
      res.try_emplace(std::move(key), std::move(valobj));

      skip_whitespace(i);

      if (json[i] == ',') {
        i += 1;
      }
    }
  }

  return {JSONObject{std::monostate{}}, 0};
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
