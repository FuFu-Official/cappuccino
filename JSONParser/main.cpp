#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

struct JSONObject {
  std::variant<std::monostate,                             // bool
               bool,                                       // true & false
               int,                                        // 3
               double,                                     // 3,14
               std::string,                                // "hello"
               std::vector<JSONObject>,                    // [true, 3]
               std::unordered_map<std::string, JSONObject> // {"hello": 3}
               >
      inner;
};

JSONObject parse(std::string_view json) {
  // TODO:
}

int main(int argc, char *argv[]) {
  std::string str = "33";
  return 0;
}
